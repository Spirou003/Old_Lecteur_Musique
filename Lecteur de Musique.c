#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <FMOD/fmod.h>
#include <FMOD/fmod_errors.h>
#include <dirent.h>
#include <errno.h>
#ifndef WIN32
    #include <sys/types.h>
#endif
#include <PJ_lib/setPixel.h>
#include <PJ_lib/ferrno.h>
#include <PJ_lib/blitNombre.h>
/*/
Reste à faire:
-Sélection à la seconde de l'emplacement dans la lecture de la musique
   Clavier: flèches et chiffres pour sélectionner, spacebar pour activer le menu, spacebar et enter pour confirmer, delete et suppr pour annuler
   Souris: clic dedans pour sélectionner, droite en dehors pour sortir, gauche pour up, droite pour dézup, roulette pour modif, gauche en dehors pour confirmer
/**/

typedef struct dirent DIRENT;

//void ferrno(FILE* fichier, char chaine[]);
int loadimages(FILE* fichier);
int freeimages(void);
void blitbouttons(int survole, int clique);
int trouveboutton(int x, int y);
//void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 blitspectre(Uint32 intervalle, void *parametre);

SDL_Surface* s_ecran = NULL;
SDL_Surface* s_rond = NULL;
SDL_Surface* s_rondenf = NULL;
SDL_Surface* s_rondsurv = NULL;
SDL_Surface* s_rondsurvenf = NULL;
SDL_Surface* s_lire = NULL;
SDL_Surface* s_pause = NULL;
SDL_Surface* s_prec = NULL;
SDL_Surface* s_suiv = NULL;
SDL_Surface* s_voldim = NULL;
SDL_Surface* s_volboo = NULL;
SDL_Surface* s_volmax = NULL;
SDL_Surface* s_volmed = NULL;
SDL_Surface* s_volmin = NULL;
SDL_Surface* s_volnul = NULL;
SDL_Surface* s_random = NULL;
SDL_Surface* s_repeat = NULL;
SDL_Surface* s_croix = NULL;
SDL_Surface* s_slash = NULL;
SDL_Surface* s_icone = NULL;
SDL_Surface* s_replay = NULL;
SDL_Surface* s_doublepoint = NULL;
SDL_Surface *s_chiffre[11] = {NULL};
//------------------------------------
SDL_Rect posbutton;
SDL_Rect poscadre2;
SDL_Rect poscadre;
double volume = 100;
int novolume = 0, lecture = 0, random = 0, repeat = 0, selectiontemps = 0;
int tempstotal[4] = {0};
int tempslu[4] = {0};
int tempsel[4] = {0};
char nomfichier[120] = "";
float *ffmod_spectre = NULL;
    
int main (int argc, char *argv[])
{
    FILE* f_fichier = NULL;
    //--------------------------------
    DIR* dir_repert = NULL;
    DIRENT* dir_fichierlu = NULL;
    //--------------------------------
    SDL_Event event;
    //--------------------------------
    FSOUND_STREAM* fmod_musique = NULL;
    //--------------------------------
    int i = 0, k = 0, alea = 0, j = 0, continuer = 0, clic = 0, okclic = 0, mclic = 0, cclic = 0, nclic = 0, newmusic = 0, mst = 0, mstc = 0, msl = 0, mslc = 0;
    int ok = 1, jesaispas = 0, appui = 0;
    //--------------------------------
    srand(time(NULL));
    dir_repert= opendir("repertoire");
    f_fichier = fopen("error.txt", "w+");
    fprintf(f_fichier, "Open directory \"repertoire\": \t");
    if (dir_repert != NULL)
    {
       fprintf(f_fichier, "Success!\nInitializing SDL: \t\t");
       if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
       {
          fprintf(f_fichier, "Success!\nInitializing FMOD: \t\t");
          if (FSOUND_Init(44100, 32, 0))
          {
             FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), 1);
             fprintf(f_fichier, "Success!\nGenerating window: \t\t");
             s_icone = IMG_Load("images/icon.png");
             if (s_icone != NULL)
             {
                SDL_WM_SetIcon(s_icone, NULL);
             }
             if ((s_ecran = SDL_SetVideoMode(512, 122, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) != NULL)
             {
                SDL_WM_SetCaption("Lecteur de Musique", NULL);
                fprintf(f_fichier, "Success!\n");
                if (loadimages(f_fichier) == 1)
                {
                poscadre.x = 320;
                poscadre.y = 103;
                poscadre.h = 16;
                poscadre.w = 40;
                poscadre2.x = 321;
                poscadre2.y = 104;
                poscadre2.h = 14;
                poscadre2.w = 38;
                SDL_FillRect(s_ecran, &poscadre, SDL_MapRGB(s_ecran->format, 255, 255, 255));
                SDL_FillRect(s_ecran, &poscadre2, SDL_MapRGB(s_ecran->format, 0, 0, 0));
                poscadre.w = 8;
                poscadre.h = 14;
                poscadre.y = 104;
                while ((dir_fichierlu = readdir(dir_repert)) != NULL)
                {
                   if (i > 1)
                   {
                      sprintf(nomfichier, "repertoire/%s", dir_fichierlu->d_name);
                      fmod_musique = FSOUND_Stream_Open(nomfichier, 0, 0, 0);
                      fprintf(f_fichier, "Load %s: ", nomfichier);
                      if (fmod_musique != NULL)
                      {
                         FSOUND_Stream_Close(fmod_musique);
                         fprintf(f_fichier, "\tSuccess!\n");
                      }
                      else
                      {
                          ok = 0;
                         fprintf(f_fichier, "Error: %s\n", FMOD_ErrorString(FSOUND_GetError()));
                      }
                   }
                   i++;
                }
                i=i-2;
                if (i == 0)
                {
                   ok = 0;
                   fprintf(f_fichier, "No music in the folder \"repertoire\"\nEnd of the application...");
                }
                if (ok == 1)
                {
                   rewinddir(dir_repert);
                   dir_fichierlu = readdir(dir_repert);
                   dir_fichierlu = readdir(dir_repert);
                   dir_fichierlu = readdir(dir_repert);
                   sprintf(nomfichier, "repertoire/%s", dir_fichierlu->d_name);
                   fmod_musique = FSOUND_Stream_Open(nomfichier, 0, 0, 0);
                   alea = 0;
                }
                while (continuer == 0 && ok == 1)
                {
                   blitspectre(1, &i);
                   blitbouttons(trouveboutton(event.motion.x, event.motion.y),okclic);
                   SDL_Flip(s_ecran);
                   SDL_PollEvent(&event);
                   SDL_Delay(20);
                   if (newmusic == 1)
                   {
                      newmusic = 0;
                      FSOUND_Stream_Stop(fmod_musique);
                      FSOUND_Stream_Close(fmod_musique);
                      rewinddir(dir_repert);
                      if (random == 1)
                      {
                         alea = ((rand())%i)+2;
                         j = 0;
                         while (j != alea)
                         {
                            dir_fichierlu = readdir(dir_repert);
                            j++;
                         }
                      }
                      else
                      {
                          j = 0;
                          if (jesaispas == 7)
                          {
                             alea++;
                             if ((alea >= i+2 && repeat == 1) || alea < 2)
                             {
                                alea = 2;
                             }
                          }
                          else if (jesaispas == 4)
                          {
                             alea--;
                             if (alea < 2 || alea > i+1)
                             {
                                alea = i+1;
                             }
                          }
                          if (alea > 1+i)
                          {
                             alea = i+1;
                          }
                          while (j != alea && alea < i+2)
                          {
                             dir_fichierlu = readdir(dir_repert);
                             j++;
                          }
                      }
                      if (alea < i+2)
                      {
                         dir_fichierlu = readdir(dir_repert);
                         sprintf(nomfichier, "repertoire/%s", dir_fichierlu->d_name);
                         fmod_musique = FSOUND_Stream_Open(nomfichier, 0, 0, 0);
                      }
                      ok = 0;
                      if (fmod_musique != NULL)
                      {
                         ok = 1;
                         sprintf(nomfichier, "Lecteur de Musique - %s", dir_fichierlu->d_name);
                         SDL_WM_SetCaption(nomfichier, NULL);
                         FSOUND_Stream_Play(FSOUND_FREE, fmod_musique);
                         if (lecture == 0)
                         {
                            FSOUND_SetPaused(FSOUND_ALL, 1);
                         }
                         mst = FSOUND_Stream_GetLengthMs(fmod_musique);
                         mstc = mst / 1000;
                         if (mstc > 5999)
                         {
                            mstc = 5999;
                         }
                         tempstotal[3] = mstc%10;
                         tempstotal[2] = (mstc%60)/10;
                         tempstotal[1] = (mstc%600)/60;
                         tempstotal[0] = (mstc%6000)/600;
                      }
                   }
                   if (fmod_musique != NULL)
                   {
                      msl = FSOUND_Stream_GetTime(fmod_musique);
                      mslc = msl / 1000;
                      if (mslc > 5999)
                      {
                         mslc = 5999;
                      }
                      tempslu[3] = mslc%10;
                      tempslu[2] = (mslc%60)/10;
                      tempslu[1] = (mslc%600)/60;
                      tempslu[0] = (mslc%6000)/600;
                      if (msl == mst)
                      {
                         newmusic = 1;
                         jesaispas = 7;
                         if (alea == i+1 && repeat == 0)
                         {
                            lecture = 0;
                            k = 1;
                            alea = 1;
                         }
                      }
                   }
                   if (event.type == SDL_QUIT || event.type == SDLK_ESCAPE || ok == 0)
                   {
                      continuer++;
                   }
                   else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                   {
                      cclic = trouveboutton(event.button.x, event.button.y);
                      if (cclic != 50 && clic == 0)
                      {
                         okclic = cclic;
                      }
                      if (okclic == 1 || okclic == 3)
                      {
                         mclic = okclic;
                      }
                      clic = 1;
                   }
                   else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
                   {
                      mclic = 0;
                      nclic = trouveboutton(event.motion.x, event.motion.y);
                      if (nclic > 0 && nclic < 50 && nclic == okclic)
                      {
                         if (okclic == 2 && volume != 0)
                         {
                            FSOUND_SetVolume(FSOUND_ALL, (25.5*((int)volume))*(novolume));
                            novolume = 1 - (novolume%2);
                         }
                         else if (okclic == 4 || okclic == 7)
                         {
                            newmusic = 1;
                            jesaispas = okclic;
                            k = 0;
                         }
                         else if (okclic == 5)
                         {
                            FSOUND_Stream_SetTime(fmod_musique, 0);
                         }
                         else if (okclic == 6)
                         {
                            FSOUND_SetPaused(FSOUND_ALL, lecture);
                            lecture = 1 - (lecture%2);
                            k = 0;
                         }
                         else if (okclic == 8)
                         {
                            repeat = 1 - (repeat%2);
                         }
                         else if (okclic == 9)
                         {
                            random = 1 - (random%2);
                         }
                      }
                      clic = 0;
                      okclic = 50;
                      cclic = 50;
                   }
                   if (selectiontemps == 0)
                   {
                   if (event.type == SDL_KEYDOWN)
                   {
                      if (appui == 0)
                      {
                         //fprintf(stderr, "%d", event.key.keysym.sym);
                         if (event.key.keysym.sym == SDLK_p)
                         {
                            FSOUND_SetPaused(FSOUND_ALL, lecture);
                            lecture = 1 - (lecture%2);
                         }
                         else if (event.key.keysym.sym == SDLK_l)
                         {
                            random = 1 - (random%2);
                         }
                         else if (event.key.keysym.sym == SDLK_r)
                         {
                            FSOUND_Stream_SetTime(fmod_musique, 0);
                         }
                         else if (event.key.keysym.sym == SDLK_LEFT)
                         {
                            jesaispas = 4;
                            newmusic = 1;
                         }
                         else if (event.key.keysym.sym == SDLK_RIGHT)
                         {
                            jesaispas = 7;
                            newmusic = 1;
                         }
                         else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                         {
                            repeat = 1 - (repeat%2);
                         }
                         else if (event.key.keysym.sym == 59)
                         {
                            novolume = 1 - (novolume%2);
                         }
                         else if (event.key.keysym.sym == SDLK_SPACE)
                         {
                            selectiontemps = 1;
                         }
                      }
                      if ((event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_KP_PLUS || event.key.keysym.sym == SDLK_UP) && novolume == 0)
                      {
                         volume = volume + 0.5;
                         if (volume > 100)
                         {
                            volume = 100;
                         }
                         FSOUND_SetVolume(FSOUND_ALL, (2.55*((int)volume)));
                      }
                      else if ((event.key.keysym.sym == 61 || event.key.keysym.sym == SDLK_KP_MINUS || event.key.keysym.sym == SDLK_DOWN) && novolume == 0)
                      {
                         volume = volume - 0.5;
                         if (volume < 0)
                         {
                            volume = 0;
                         }
                         FSOUND_SetVolume(FSOUND_ALL, (2.55*((int)volume)));
                      }
                      appui = 1;
                   }
                   if (event.type == SDL_KEYUP)
                   {
                      appui = 0;
                   }
                   }
                   else
                   {
                      if (event.type == SDL_KEYDOWN)
                      {
                         if (appui == 0)
                         {
                            if (event.key.keysym.sym == SDLK_LEFT)
                            {
                               selectiontemps--;
                               if (selectiontemps < 1)
                               {
                                  selectiontemps = 4;
                               } 
                            }
                            else if (event.key.keysym.sym == SDLK_RIGHT)
                            {
                               selectiontemps++;
                               if (selectiontemps > 4)
                               {
                                  selectiontemps = 1;
                               }
                            }
                            else if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                            {
                               FSOUND_Stream_SetTime(fmod_musique, 1000*(tempsel[3]+tempsel[2]*10+tempsel[1]*60+tempsel[0]*600));
                               selectiontemps = 0;
                            }
                            else if (event.key.keysym.sym == SDLK_DELETE || event.key.keysym.sym == 8)
                            {
                               selectiontemps = 0;
                            }
                            else if (event.key.keysym.sym >= SDLK_KP0 && event.key.keysym.sym <= SDLK_KP9)
                            {
                               tempsel[selectiontemps-1] = event.key.keysym.sym - SDLK_KP0;
                            }
                            else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9)
                            {
                               tempsel[selectiontemps-1] = event.key.keysym.sym - SDLK_0;
                            }
                            else if (event.key.keysym.sym == SDLK_UP)
                            {
                               tempsel[selectiontemps-1]++;
                               if (tempsel[selectiontemps-1] > 9)
                               {
                                  tempsel[selectiontemps-1] = 0;
                               }
                               else if (tempsel[2] > 5)
                               {
                                  tempsel[2] = 0;
                               }
                            }
                            else if (event.key.keysym.sym == SDLK_DOWN)
                            {
                               tempsel[selectiontemps-1]--;
                               if (tempsel[2] < 0)
                               {
                                  tempsel[2] = 5;
                               }
                               if (tempsel[selectiontemps-1] < 0)
                               {
                                  tempsel[selectiontemps-1] = 9;
                               }
                            }
                            if (tempsel[2] > 5)
                            {
                               tempsel[2] = 5;
                            }
                            if (tempsel[0]*600 + tempsel[1]*60 + tempsel[2]*10 + tempsel[3] > mstc)
                            {
                               tempsel[0] = tempstotal[0];
                               tempsel[1] = tempstotal[1];
                               tempsel[2] = tempstotal[2];
                               tempsel[3] = tempstotal[3];
                            }
                         }
                         appui = 1;
                      }
                      else if (event.type == SDL_KEYUP)
                      {
                         appui = 0;
                      }
                      //else if 
                   }
                   if (mclic == 3 && volume < 100 && novolume == 0)
                   {
                      volume = volume + 0.5;
                      FSOUND_SetVolume(FSOUND_ALL, (2.55*((int)volume)));
                   }
                   else if (mclic == 1 && volume > 0 && novolume == 0)
                   {
                      volume = volume - 0.5;
                      FSOUND_SetVolume(FSOUND_ALL, (2.55*((int)volume)));
                   }
                   if (random == 1)
                   {
                      repeat = 1;
                   }
                   if (k == 1)
                   {
                      lecture = 0;
                   }
                }
                if (ok == 0)
                {
                   if (i > 0)
                   {
                   fprintf(f_fichier, "Error: %s\nEnd of the application...", FMOD_ErrorString(FSOUND_GetError()));
                   }
                   fclose(f_fichier);
                   system("error.txt");
                }
                }
                else
                {
                    fclose(f_fichier);
                    system("error.txt");
                }
             }
             else
             {
                 fprintf(f_fichier, "Error: %s\nEnd of the application...", SDL_GetError());
                 fclose(f_fichier);
                 system("error.txt");
             }
             FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), 0);
             FSOUND_Stream_Close(fmod_musique);
             FSOUND_Close();
          }
          else
          {
              fprintf(f_fichier, "Error: %s\nEnd of the application...", FMOD_ErrorString(FSOUND_GetError()));
              fclose(f_fichier);
              system("error.txt");
          }
          freeimages();
          SDL_Quit();
       }
       else
       {
           fprintf(f_fichier, "Error: %s\nEnd of the application...", SDL_GetError());
           fclose(f_fichier);
           system("error.txt");
       }
       closedir(dir_repert);
    }
    else
    {
        ferrno(f_fichier, "Error");
        fprintf(f_fichier, "End of the application...");
        fclose(f_fichier);
        system("error.txt");
    }
	return 0;
}

Uint32 blitspectre(Uint32 intervalle, void *parametre)
{
   int i = 0, j = 0, k = 0, r = 0, g = 85, b = 255;
   SDL_Rect booo;
   booo.x = 0;
   booo.y = 0;
   booo.w = 512;
   booo.h = 101;
   ffmod_spectre = FSOUND_DSP_GetSpectrum();
   SDL_FillRect(s_ecran, &booo, SDL_MapRGB(s_ecran->format, 0, 0, 0));
   SDL_LockSurface(s_ecran);
   while (i < 512)
   {
      j = 0;
      k = (sqrt(ffmod_spectre[i]))*100;
      if (k > 100)
      {
         k = 100;
      }
      while (j < 100)
      {
         if (j >= k)
         {
            j = 100;
         }
         else
         {
            g = 170 - j*170/k;
            setPixel(s_ecran, i, 99-j, SDL_MapRGB(s_ecran->format, 0, g, b));
         }
         j++;
      }
      i++;
   }
   SDL_UnlockSurface(s_ecran);
   free(ffmod_spectre);
   return intervalle;
}

int trouveboutton(int x, int y)
{
    int xt = 0, yt = 0, i = 0, sommex = 0, sommey = 0, detx = 0, dety = 0;
    if (y < 101 || /**/y == 121 ||/**/ x > 225)
    {
       return 50;
    }
    yt = y - 101;
    if (x < 42)
    {
       xt = x%21;
       i = (x/21) + 1;
       if (xt == 0)
       {
          return 50;
       }
    }
    else if (x > 93 && x < 177)
    {
       xt = (x-93)%21;
       i = ((x-93)/21)+4;
       if (xt == 0)
       {
          return 50;
       }
    }
    else if (x > 67 && x < 88)
    {
       xt = x - 67;
       i = 3;
    }
    else if (x > 182)
    {
       xt = (x - 182)%21;
       i = ((x-182)/21)+8;
       if (xt == 0)
       {
          return 50;
       }
    }
    else
    {
        return 50;
    }
    sommex = yt + xt;
    if (sommex < 7 || sommex > 33)
    {
       i = 50;
    }
    detx = yt;
    dety = 21-xt;
    sommey = detx + dety;
    if (sommey < 7 || sommey > 33)
    {
       i = 50;
    }
    return i;
}

void blitbouttons(int survole, int clique)
{
   SDL_Rect position;
   int i = 0, j = 100;
   position.x = 1;
   position.y = 101;
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 1 && clique == 1)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 1)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 1)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   SDL_BlitSurface(s_voldim, NULL, s_ecran, &position);
   //-----------------------------------------------------------
   position.x = 22;
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 2 && clique == 2)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 2)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 2)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   if (((int)volume) == 0 || novolume == 1)
   {
      SDL_BlitSurface(s_volnul, NULL, s_ecran, &position);
   }
   else if (((int)volume) > 69)
   {
      SDL_BlitSurface(s_volmax, NULL, s_ecran, &position);
   }
   else if (((int)volume) < 70 && ((int)volume) > 29)
   {
      SDL_BlitSurface(s_volmed, NULL, s_ecran, &position);
   }
   else
   {
       SDL_BlitSurface(s_volmin, NULL, s_ecran, &position);
   }
   //-----------------------------------------------------------
   position.x = 43;
   position.y = 105;
   blitNombre(s_chiffre, &position, 8, volume, s_ecran);
   /*/while (i < 3)
   {
      if (novolume == 0)
      {
         SDL_BlitSurface(s_chiffre[((((int)volume)/j)%10)], NULL, s_ecran, &position);
         if (((((int)volume)/j)%10) == 0 && ((int)volume) < j)
         {
            SDL_BlitSurface(s_chiffre[10], NULL, s_ecran, &position);
         }
      }
      else
      {
          if (i == 0 || i == 1)
          {
             SDL_BlitSurface(s_chiffre[10], NULL, s_ecran, &position);
          }
          else
          {
              SDL_BlitSurface(s_chiffre[0], NULL, s_ecran, &position);
          }
      }
      if (((int)volume) == 0 && i == 2)
      {
         SDL_BlitSurface(s_chiffre[0], NULL, s_ecran, &position);
      }
      position.x = position.x + 8;
      j = j/10;
      i++;
   }/**/
   position.x = 68;
   position.y = 101;
   //-----------------------------------------------------------
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 3 && clique == 3)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 3)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 3)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   SDL_BlitSurface(s_volboo, NULL, s_ecran, &position);
   position.x = 94;
   //-----------------------------------------------------------
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 4 && clique == 4)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 4)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 4)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   SDL_BlitSurface(s_prec, NULL, s_ecran, &position);
   position.x = 115;
   //-----------------------------------------------------------
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 5 && clique == 5)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 5)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 5)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   SDL_BlitSurface(s_replay, NULL, s_ecran, &position);
   position.x = 136;
   //-----------------------------------------------------------
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 6 && clique == 6)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 6)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 6)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   if (lecture == 0)
   {
      SDL_BlitSurface(s_lire, NULL, s_ecran, &position);
   }
   else
   {
       SDL_BlitSurface(s_pause, NULL, s_ecran, &position);
   }
   position.x = 157;
   //-----------------------------------------------------------
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 7 && clique == 7)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 7)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 7)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   SDL_BlitSurface(s_suiv, NULL, s_ecran, &position);
   position.x = 183;
   //-----------------------------------------------------------
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 8 && clique == 8)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 8)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 8)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   SDL_BlitSurface(s_repeat, NULL, s_ecran, &position);
   if (repeat == 0)
   {
      SDL_BlitSurface(s_croix, NULL, s_ecran, &position);
   }
   position.x = 204;
   //-----------------------------------------------------------
   SDL_BlitSurface(s_rond, NULL, s_ecran, &position);
   if (survole == 9 && clique == 9)
   {
      SDL_BlitSurface(s_rondsurvenf, NULL, s_ecran, &position);
   }
   else if (survole == 9)
   {
      SDL_BlitSurface(s_rondsurv, NULL, s_ecran, &position);
   }
   else if (clique == 9)
   {
      SDL_BlitSurface(s_rondenf, NULL, s_ecran, &position);
   }
   SDL_BlitSurface(s_random, NULL, s_ecran, &position);
   if (random == 0)
   {
      SDL_BlitSurface(s_croix, NULL, s_ecran, &position);
   }
   i = 0;
   SDL_FillRect(s_ecran, &poscadre2, SDL_MapRGB(s_ecran->format, 0, 0, 0));
   position.x = 268;
   position.y = 105;
   SDL_BlitSurface(s_slash, NULL, s_ecran, &position);
   position.x = 246;
   SDL_BlitSurface(s_doublepoint, NULL, s_ecran, &position);
   position.x = 291;
   SDL_BlitSurface(s_doublepoint, NULL, s_ecran, &position);
   position.x = 337;
   SDL_BlitSurface(s_doublepoint, NULL, s_ecran, &position);
   while (i < 12)
   {
      if (i < 4)
      {
         position.x = 230 + 8*i + (i/2)*6;
         SDL_BlitSurface(s_chiffre[tempslu[i]], NULL, s_ecran, &position);
      }
      else if (i > 3 && i < 8)
      {
         position.x = 275 + 8*(i-4) + ((i%4)/2)*6;
         SDL_BlitSurface(s_chiffre[tempstotal[i-4]], NULL, s_ecran, &position);
      }
      else
      {
         position.x = 321 + 8*(i-8) + ((i%4)/2)*6;
         if (selectiontemps == (i-7))
         {
            poscadre.x = position.x;
            SDL_FillRect(s_ecran, &poscadre, SDL_MapRGB(s_ecran->format, 255, 255, 0));
         }
         SDL_BlitSurface(s_chiffre[tempsel[i-8]], NULL, s_ecran, &position);
      }
      i++;
   }
}

int loadimages(FILE* fichier)
{
    int i = 0, j = 1;
    char chaine[13] = "images/0.png";
    fprintf(fichier, "Load images/rond.png: \t\t");
    if ((s_rond = IMG_Load("images/rond.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/rond2.png: \t\t");
    if ((s_rondenf = IMG_Load("images/rond2.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/rond3.png: \t\t");
    if ((s_rondsurv = IMG_Load("images/rond3.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/rond4.png: \t\t");
    if ((s_rondsurvenf = IMG_Load("images/rond4.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/lire.png: \t\t");
    if ((s_lire = IMG_Load("images/lire.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/pause.png: \t\t");
    if ((s_pause = IMG_Load("images/pause.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/prec.png: \t\t");
    if ((s_prec = IMG_Load("images/prec.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/suiv.png: \t\t");
    if ((s_suiv = IMG_Load("images/suiv.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/voldim.png: \t");
    if ((s_voldim = IMG_Load("images/voldim.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/volboo.png: \t");
    if ((s_volboo = IMG_Load("images/volboo.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/volmax.png: \t");
    if ((s_volmax = IMG_Load("images/volmax.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/volmed.png: \t");
    if ((s_volmed = IMG_Load("images/volmed.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/volmin.png: \t");
    if ((s_volmin = IMG_Load("images/volmin.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/volnul.png: \t");
    if ((s_volnul = IMG_Load("images/volnul.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/repeat.png: \t");
    if ((s_repeat = IMG_Load("images/repeat.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/random.png: \t");
    if ((s_random = IMG_Load("images/random.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/croix.png: \t\t");
    if ((s_croix = IMG_Load("images/croix.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/slash.png: \t\t");
    if ((s_slash = IMG_Load("images/slash.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/replay.png: \t");
    if ((s_replay = IMG_Load("images/replay.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "Load images/punt.png: \t\t");
    if ((s_doublepoint = IMG_Load("images/punt.png")) != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    while (i < 10)
    {
       chaine[7] = ('0' + i);
       fprintf(fichier, "Load %s: \t\t", chaine);
       if ((s_chiffre[i] = IMG_Load(chaine)) != NULL)
       {
          fprintf(fichier, "Success!\n");
       }
       else
       {
          fprintf(fichier, "Failure!\n");
          j = 0;
       }
       i++;
    }
    if ((s_chiffre[10] = SDL_CreateRGBSurface(SDL_HWSURFACE, 8, 12, 32, 0, 0, 0, 0)) != NULL)
    {
       i++;
    }
    fprintf(fichier, "Load images/icon.png: \t\t");
    if (s_icone != NULL)
    {
       fprintf(fichier, "Success!\n");
    }
    else
    {
       fprintf(fichier, "Failure!\n");
       j = 0;
    }
    fprintf(fichier, "------------------------------------------------------------------------\n");
    return j;
}

int freeimages(void)
{
    int i = 0, j = 0;
    if (s_rond != NULL)
    {
       SDL_FreeSurface(s_rond);
    }
    if (s_rondenf != NULL)
    {
       SDL_FreeSurface(s_rondenf);
    }
    if (s_rondsurv != NULL)
    {
       SDL_FreeSurface(s_rondsurv);
    }
    if (s_rondsurvenf != NULL)
    {
       SDL_FreeSurface(s_rondsurvenf);
    }
    if (s_lire != NULL)
    {
       SDL_FreeSurface(s_lire);
    }
    if (s_pause != NULL)
    {
       SDL_FreeSurface(s_pause);
    }
    if (s_prec != NULL)
    {
       SDL_FreeSurface(s_prec);
    }
    if (s_suiv != NULL)
    {
       SDL_FreeSurface(s_suiv);
    }
    if (s_voldim != NULL)
    {
       SDL_FreeSurface(s_voldim);
    }
    if (s_volboo != NULL)
    {
       SDL_FreeSurface(s_volboo);
    }
    if (s_volmax != NULL)
    {
       SDL_FreeSurface(s_volmax);
    }
    if (s_volmed != NULL)
    {
       SDL_FreeSurface(s_volmed);
    }
    if (s_volmin != NULL)
    {
       SDL_FreeSurface(s_volmin);
    }
    if (s_volnul != NULL)
    {
       SDL_FreeSurface(s_volnul);
    }
    if (s_repeat != NULL)
    {
       SDL_FreeSurface(s_repeat);
    }
    if (s_random != NULL)
    {
       SDL_FreeSurface(s_random);
    }
    if (s_croix != NULL)
    {
       SDL_FreeSurface(s_croix);
    }
    if (s_slash != NULL)
    {
       SDL_FreeSurface(s_slash);
    }
    if (s_icone != NULL)
    {
       SDL_FreeSurface(s_icone);
    }
    if (s_icone != NULL)
    {
       SDL_FreeSurface(s_replay);
    }
    if (s_doublepoint != NULL)
    {
       SDL_FreeSurface(s_doublepoint);
    }
    while (i < 11)
    {
       if (s_chiffre[i] != NULL)
       {
          SDL_FreeSurface(s_chiffre[i]);
       }
       i++;
    }
}

/*/void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;

    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}/**/

/*/void ferrno(FILE* fichier, char chaine[])
{
     switch(errno)
     {
        case EPERM:
             fprintf(fichier, "%s: Operation not permitted", chaine);
             break;
        case ENOFILE:
             fprintf(fichier, "%s: No such file or directory", chaine);
             break;
        case ESRCH:
             fprintf(fichier, "%s: No such process", chaine);
             break;
        case EINTR:
             fprintf(fichier, "%s: Interrupted function call", chaine);
             break;
        case EIO:
             fprintf(fichier, "%s: Input/output error", chaine);
             break;
        case ENXIO:
             fprintf(fichier, "%s: No such device or address", chaine);
             break;
        case E2BIG:
             fprintf(fichier, "%s: Arg list too long", chaine);
             break;
        case ENOEXEC:
             fprintf(fichier, "%s: Exec format error", chaine);
             break;
        case EBADF:
             fprintf(fichier, "%s: Bad file descriptor", chaine);
             break;
        case ECHILD:
             fprintf(fichier, "%s: No child processes", chaine);
             break;
        case EAGAIN:
             fprintf(fichier, "%s: Resource temporarily unavailable", chaine);
             break;
        case ENOMEM:
             fprintf(fichier, "%s: Not enough space", chaine);
             break;
        case EACCES:
             fprintf(fichier, "%s: Permission denied", chaine);
             break;
        case EFAULT:
             fprintf(fichier, "%s: Bad address", chaine);
             break;
        case 15:
             fprintf(fichier, "%s: Unknown Error", chaine);
             break;
        case EBUSY:
             fprintf(fichier, "%s: strerror reports \"Resource device\"", chaine);
             break;
        case EEXIST:
             fprintf(fichier, "%s: File exists", chaine);
             break;
        case EXDEV:
             fprintf(fichier, "%s: Improper link (cross-device link?)", chaine);
             break;
        case ENODEV:
             fprintf(fichier, "%s: No such device", chaine);
             break;
        case ENOTDIR:
             fprintf(fichier, "%s: Not a directory", chaine);
             break;
        case EISDIR:
             fprintf(fichier, "%s: Is a directory", chaine);
             break;
        case EINVAL:
             fprintf(fichier, "%s: Invalid argument", chaine);
             break;
        case ENFILE:
             fprintf(fichier, "%s: Too many open files in system", chaine);
             break;
        case EMFILE:
             fprintf(fichier, "%s: Too many open files", chaine);
             break;
        case ENOTTY:
             fprintf(fichier, "%s: Inappropriate I/O control operation", chaine);
             break;
        case 26:
             fprintf(fichier, "%s: Unknown Error", chaine);
             break;
        case EFBIG:
             fprintf(fichier, "%s: File too large", chaine);
             break;
        case ENOSPC:
             fprintf(fichier, "%s: No space left on device", chaine);
             break;
        case ESPIPE:
             fprintf(fichier, "%s: Invalid seek (seek on a pipe?)", chaine);
             break;
        case EROFS:
             fprintf(fichier, "%s: Read-only file system", chaine);
             break;
        case EMLINK:
             fprintf(fichier, "%s: Too many links", chaine);
             break;
        case EPIPE:
             fprintf(fichier, "%s: Broken pipe", chaine);
             break;
        case EDOM:
             fprintf(fichier, "%s: Domain error (math functions)", chaine);
             break;
        case ERANGE:
             fprintf(fichier, "%s: Result too large (possibly too small)", chaine);
             break;
        case 35:
             fprintf(fichier, "%s: Unknown Error", chaine);
             break;
        case EDEADLOCK:
             fprintf(fichier, "%s: Resource deadlock avoided (non-Cyg)", chaine);
             break;
        case 37:
             fprintf(fichier, "%s: Unknown Error", chaine);
             break;
        case ENAMETOOLONG:
             fprintf(fichier, "%s: Filename too long (91 in Cyg?)", chaine);
             break;
        case ENOLCK:
             fprintf(fichier, "%s: No locks available (46 in Cyg?)", chaine);
             break;
        case ENOSYS:
             fprintf(fichier, "%s: Function not implemented (88 in Cyg?)", chaine);
             break;
        case ENOTEMPTY:
             fprintf(fichier, "%s: Directory not empty (90 in Cyg?)", chaine);
             break;
        case EILSEQ:
             fprintf(fichier, "%s: Illegal byte sequence", chaine);
             break;
     }
     fprintf(fichier, "\n");
}/**/




/*/
printf("%s", fichierlu->d_name);
musique = FSOUND_Stream_Open(nomdemusique, 0, débutmusique, partielue);       charger musique
FSOUND_Stream_Play(FSOUND_FREE, musique);                                     lire musique
FSOUND_SetVolume(FSOUND_ALL, 120);	                                          FSOUND_ALL (tous les sons), 120: nombre de 0 (rien) à 255 (max)
FSOUND_Stream_Stop(musique);                                                  stopper lecture
FSOUND_Stream_Close(musique);                                                 libérer mémoire


/**/



/*/
11.1. Obtenir la durée totale du MP3


int iLength; FSOUND_STREAM *MP3Stream; iLength = FSOUND_Stream_GetLengthMs(MP3Stream);

FSOUND_GetLengthMs prend en paramètre le stream MP3 (obtenu lors de l'ouverture du MP3) et retourne la longueur du morceau, exprimée en milli-secondes.


11.2. Obtenir la durée écoulée


int iPassed; FSOUND_STREAM *MP3Stream; iPassed = FSOUND_Stream_GetTime(MP3Stream);

FSOUND_Stream_GetTime prend en parmètre le stream MP3 (obtenu lors de l'ouverture du MP3) et retourne la durée écoulée, exprimée en milli-secondes.


11.3. Se déplacer dans le MP3


bool lRet; int iPos; FSOUND_STREAM *MP3Stream; lRet = FSOUND_Stream_SetTime(MP3Stream, iPos);

FSOUND_Stream_SetTime prend en 1er parmètre le stream MP3 (obtenu lors de l'ouverture du MP3) et en second, la position désirée, exprimée en milli-secondes.

La valeur de retour est true si le positionnement a eu lieu, false sinon. 
/**/

