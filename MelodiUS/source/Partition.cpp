#include "Partition.h"
NoteType GetNoteValueValue(double d, NoteValue t);


int Partition::ajoutLigne()
{
    feuille.push_back(Portee(nbsLigne, m_parent));
    nbsLigne++;
    spinBox.raise();
    spinBox_2.raise();
    return nbsLigne;
}

void Partition::ecrireMusique(std::vector<Note> vecNote)
{
    nom        = spinBox.value();
    denom      = spinBox_2.value();
    int ligne  = 0;
    int mesure = 0;

    compo.push_back(std::vector<Note>());
    std::array<double, 5> valNoteComparasion = {4, 2, 1, 0.5};
    if((nom != 1 && nom != 2 && nom != 4 && nom != 8 && nom != 16)
       && (denom != 1 && denom != 2 && denom != 3 && denom != 4 && denom != 5 && denom != 6
           && denom != 7 && denom != 8))
    {
        // QErrorMessage() n(&m_parent);
    }
    int valeurParMesure = nom * (4 / denom);


    //=============== Boute de code qui ce repete pour chaque note du vecteur et qui transforme le
    // tout en note prete a etre mise dans une portee============//
    for(int i = 0; i < vecNote.size(); i++)
    {
        // On commence par verifier si la mesure est pleine et si oui on rajoute une mesuere
        double sum = 0;
        for(int j = 0; j < compo[mesure].size(); j++)
        {
            sum += compo[mesure][j].getNoteSum();
        }
        if(sum >= valeurParMesure)
        {
            mesure++;
            compo.push_back(std::vector<Note>());
            sum = 0;
        }
        //============================================================================
        // On regarde maintenant si la note rentre telle quelle dans le vecteur
        if(sum + vecNote[i].getNoteSum() <= valeurParMesure)    // si sa rentre on ajoute la note
        {
            compo[mesure].push_back(vecNote[i]);
        }
        else    // si sa rentre pas
        {
            // on commence par rentrer ce qui renrte et definir le reste qui rentre pas
            double reste      = sum + vecNote[i].getNoteSum() - valeurParMesure;
            double resteAvant = valeurParMesure - sum;
            bool   first      = true;
            while(std::find(valNoteComparasion.begin(), valNoteComparasion.end(), resteAvant)
                  == valNoteComparasion.end())
            {
                double noteAAjouter = 0;
                if(resteAvant - 4 > 0)
                {
                    noteAAjouter = 4;
                }
                else if(resteAvant - 2 > 0)
                {
                    noteAAjouter = 2;
                }
                else if(resteAvant - 1 > 0)
                {
                    noteAAjouter = 1;
                }
                else if(resteAvant - 0.5 > 0)
                {
                    noteAAjouter = 0.5;
                }
                compo[mesure].push_back(
                  Note(GetNoteValueValue(noteAAjouter, vecNote[i].getNoteValue()),
                       vecNote[i].getNoteValue(),
                       !first));
                first      = false;
                resteAvant = resteAvant - noteAAjouter;
            }
            compo[mesure].push_back(Note(GetNoteValueValue(resteAvant, vecNote[i].getNoteValue()),
                                         vecNote[i].getNoteValue(),
                                         !first));
            mesure++;
            compo.push_back(std::vector<Note>());
            resteApres(reste, mesure, valeurParMesure, vecNote, i);
        }
    }
    
    double sum = 0;
    for(int j = 0; j < compo[mesure].size(); j++)
    {
        sum += compo[mesure][j].getNoteSum();
    }
    double resteAvant = valeurParMesure - sum;
    if(resteAvant!=0)
    {
        while(std::find(valNoteComparasion.begin(), valNoteComparasion.end(), resteAvant)
              == valNoteComparasion.end())
        {
            double noteAAjouter = 0;
            if(resteAvant - 4 > 0)
            {
                noteAAjouter = 4;
            }
            else if(resteAvant - 2 > 0)
            {
                noteAAjouter = 2;
            }
            else if(resteAvant - 1 > 0)
            {
                noteAAjouter = 1;
            }
            else if(resteAvant - 0.5 > 0)
            {
                noteAAjouter = 0.5;
            }
            compo[mesure].push_back(
              Note(GetNoteValueValue(noteAAjouter, NoteValue::UNKNOWN), NoteValue::UNKNOWN, false));
            resteAvant = resteAvant - noteAAjouter;
        }
        compo[mesure].push_back(
          Note(GetNoteValueValue(resteAvant, NoteValue::UNKNOWN), NoteValue::UNKNOWN, false));
    }
    
    for(int i = 0; i < compo.size(); i++)
    {
        for(int j = 0; j < compo[mesure].size(); j++)
        {
            if(compo[mesure][j].getNoteType() == NoteType::Croche && !compo[mesure][j].isLiee())
            {
                if(compo[mesure].size()-1!=j)
                {
                    if(compo[mesure][j + 1].getNoteType()
                       == NoteType::Croche&& !compo[mesure][j+1].isLiee())
                    {
                        compo[mesure][j + 1].setDeuxCroche(true, true);
                        compo[mesure][j].setDeuxCroche(true, false);
                        j++;
                    }
                }
                
            }
        }
    }

    //=============== Boute de code qui transforme les note en notewidget et qui affiche chaque
    // mesure a la bonne place====================//
    /* Allocates size of the composition vector in advance to avoid NoteWidget copies */
    size_t totalSize = std::accumulate(
      compo.begin(), compo.end(), 0, [](size_t sum, const std::vector<Note>& current) {
          return sum + current.size();
      });
    composition.reserve(totalSize);
    ligne          = 0;
    bool   passage = false;
    int lastX   = 0;
    int    lastY   = 0;
    NoteValue lastNoteValue;
    int    nbsNote = 0;
    for(int i = 0; i < compo.size(); i++)
    {
        if(i % 4 == 0 && passage)
        {
            ajoutLigne();
            m_parent->resize(m_parent->width(), m_parent->height() + 350);
            ligne++;
        }
        for(int j = 0; j < compo[i].size(); j++)
        {
            int posX =
              (int)((((i % 4) * 215) + 85)
                    + (((j + 1) * (200 / compo[i].size())) - (200 / compo[i].size()) + 15));
           
            if(!(i == 0 && j == 0))
            {
                lastNoteValue = composition[nbsNote - 1].getNoteValue();
                if((posX - (composition[nbsNote - 1].getX()) < 500))
                {
                    lastX = composition[nbsNote - 1].getX();
                    lastY = composition[nbsNote - 1].getY();
                }

            }
            else
            { 
                lastX = posX-125;
            }
            NoteWidget& a = composition.emplace_back(m_parent, compo[i][j], posX, ligne, lastX,lastY,lastNoteValue);
            a.show();
            nbsNote++;
        }
        passage = true;
    }
}
void Partition::resteApres(double            reste,
                           int               mesure,
                           int               valeurParMesure,
                           std::vector<Note> vecNote,
                           int               i)
{

    std::array<double, 5> valNoteComparasion = {4, 2, 1, 0.5};

    while(std::find(valNoteComparasion.begin(), valNoteComparasion.end(), reste)
          == valNoteComparasion.end())
    {
        // on regarde si la mesure suivante est pleine
        double sum = 0;
        for(int j = 0; j < compo[mesure].size(); j++)
        {
            sum += compo[mesure][j].getNoteSum();
        }

        // on regarde si sa rentre dans la mesure
        if(sum + reste >= valeurParMesure)    // si non
        {
            if(sum >= valeurParMesure)
            {
                mesure++;
                compo.push_back(std::vector<Note>());
                sum = 0;
            }
            double resteAvant   = valeurParMesure - sum;
            double noteAAjouter = 0;
            if(resteAvant - 4 > 0)
            {
                noteAAjouter = 4;
            }
            else if(resteAvant - 2 > 0)
            {
                noteAAjouter = 2;
            }
            else if(resteAvant - 1 > 0)
            {
                noteAAjouter = 1;
            }
            else if(resteAvant - 0.5 > 0)
            {
                noteAAjouter = 0.5;
            }
            compo[mesure].push_back(Note(GetNoteValueValue(noteAAjouter, vecNote[i].getNoteValue()),
                                         vecNote[i].getNoteValue(),
                                         true));
            reste = reste - noteAAjouter;

            resteApres(reste, mesure, valeurParMesure, vecNote, i);
        }
        else    // si oui
        {
            double noteAAjouter = 0;
            if(reste - 4 > 0)
            {
                noteAAjouter = 4;
            }
            else if(reste - 2 > 0)
            {
                noteAAjouter = 2;
            }
            else if(reste - 1 > 0)
            {
                noteAAjouter = 1;
            }
            else if(reste - 0.5 > 0)
            {
                noteAAjouter = 0.5;
            }
            compo[mesure].push_back(Note(GetNoteValueValue(noteAAjouter, vecNote[i].getNoteValue()),
                                         vecNote[i].getNoteValue(),
                                         true));
            reste = reste - noteAAjouter;
        }
    }
    if(reste != 0)
    {
        compo[mesure].push_back(Note(
          GetNoteValueValue(reste, vecNote[i].getNoteValue()), vecNote[i].getNoteValue(), true));
    }
}



bool Partition::mesureEstPleine()
{
    return true;
}
NoteType GetNoteValueValue(double d, NoteValue t)
{
    if(t != NoteValue::UNKNOWN)
    {
        if(d == 4)
        {
            return NoteType::Ronde;
        }
        else if(d == 2)
        {
            return NoteType::Blanche;
        }
        else if(d == 1)
        {
            return NoteType::Noire;
        }
        else if(d == 0.5)
        {
            return NoteType::Croche;
        }
        else if(d == 0.25)
        {
            return NoteType::DoubleCroche;
        }
    }
    else
    {
        if(d == 4)
        {
            return NoteType::Pause;
        }
        else if(d == 2)
        {
            return NoteType::DemiPause;
        }
        else if(d == 1)
        {
            return NoteType::Silence;
        }
        else if(d == 0.5)
        {
            return NoteType::DemiSilence;
        }
        else if(d == 0.25)
        {
            return NoteType::QuartSilence;
        }
    }
}
