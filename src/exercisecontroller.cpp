/****************************************************************************
**
** Copyright (C) 2015 by Sandro S. Andrade <sandroandrade@kde.org>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of
** the License or (at your option) version 3 or any later version
** accepted by the membership of KDE e.V. (or its successor approved
** by the membership of KDE e.V.), which shall act as a proxy
** defined in Section 14 of version 3 of the license.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "exercisecontroller.h"

#include "midisequencer.h"

#include <drumstick/alsaevent.h>

#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QStandardPaths>

#include <KI18n/KLocalizedString>

ExerciseController::ExerciseController(MidiSequencer *midiSequencer) :
    m_midiSequencer(midiSequencer),
    m_chosenExercise(0),
    m_chosenRootNote(0)
{
}

ExerciseController::~ExerciseController()
{
}

void ExerciseController::setExerciseOptions(QJsonArray exerciseOptions)
{
    m_exerciseOptions = exerciseOptions;
}

QString ExerciseController::randomlyChooseExercise()
{
    qsrand(QDateTime::currentDateTime().toTime_t());
    m_chosenExercise = qrand() % m_exerciseOptions.size();
    int minRootNote = 21;
    int maxRootNote = 104;
    int sequenceFromRoot = m_exerciseOptions[m_chosenExercise].toObject()["sequenceFromRoot"].toString().toInt();
    do
        m_chosenRootNote = minRootNote + qrand() % (maxRootNote - minRootNote);
    while (m_chosenRootNote + sequenceFromRoot > 108);

    Song *song = new Song;
    song->setHeader(0, 1, 60);
    song->setInitialTempo(600000);
    m_midiSequencer->setSong(song);
    m_midiSequencer->appendEvent(m_midiSequencer->SMFTempo(600000), 0);
    drumstick::SequencerEvent *ev;
    m_midiSequencer->appendEvent(m_midiSequencer->SMFNoteOn(1, m_chosenRootNote, 120), 0);
    m_midiSequencer->appendEvent(m_midiSequencer->SMFNoteOff(1, m_chosenRootNote, 120), 60);
    m_midiSequencer->appendEvent(ev = m_midiSequencer->SMFNoteOn(1, m_chosenRootNote + sequenceFromRoot, 120), 60);
    ev->setTag(0);
    m_midiSequencer->appendEvent(ev = m_midiSequencer->SMFNoteOff(1, m_chosenRootNote + sequenceFromRoot, 120), 120);
    ev->setTag(0);

    return m_exerciseOptions[m_chosenExercise].toObject()["name"].toString();
}

unsigned int ExerciseController::chosenRootNote()
{
    return m_chosenRootNote;
}

void ExerciseController::playChoosenExercise()
{
    m_midiSequencer->play();
}

bool ExerciseController::configureExercises()
{
    m_errorString.clear();
    QDir exercisesDir = QStandardPaths::locate(QStandardPaths::AppDataLocation, "exercises", QStandardPaths::LocateDirectory);
    foreach (QString exercise, exercisesDir.entryList(QDir::Files)) {
        QFile exerciseFile(exercisesDir.absoluteFilePath(exercise));
        if (!exerciseFile.open(QIODevice::ReadOnly)) {
            m_errorString = i18n("Couldn't open exercise file \"%1\".").arg(exercisesDir.absoluteFilePath(exercise));
            return false;
        }
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(exerciseFile.readAll(), &error);

        if (error.error != QJsonParseError::NoError) {
            m_errorString = error.errorString();
            exerciseFile.close();
            return false;
        }
        else {
            if (m_exercises.length() == 0)
                m_exercises = jsonDocument.object();
            else
                m_exercises["exercises"] = mergeExercises(m_exercises["exercises"].toArray(),
                                                        jsonDocument.object()["exercises"].toArray());
        }
        exerciseFile.close();
    }
    return true;
}

QString ExerciseController::errorString() const
{
    return m_errorString;
}

QJsonObject ExerciseController::exercises() const
{
    return m_exercises;
}

QJsonArray ExerciseController::mergeExercises(QJsonArray exercises, QJsonArray newExercises)
{
    for (QJsonArray::ConstIterator i1 = newExercises.constBegin(); i1 < newExercises.constEnd(); ++i1) {
        if (i1->isObject()) {
            QJsonArray::ConstIterator i2;
            for (i2 = exercises.constBegin(); i2 < exercises.constEnd(); ++i2) {
                if (i2->isObject() && i1->isObject() && i2->toObject()["name"] == i1->toObject()["name"]) {
                    QJsonObject jsonObject = exercises[i2-exercises.constBegin()].toObject();
                    jsonObject["children"] = mergeExercises(i2->toObject()["children"].toArray(), i1->toObject()["children"].toArray());
                    exercises[i2-exercises.constBegin()] = jsonObject;
                    break;
                }
            }
            if (i2 == exercises.constEnd())
                exercises.append(*i1);
        }
    }
    return exercises;
}