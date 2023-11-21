
/**
 * \file pgm_reader.c
 * \brief pgm picture read write file
 * \author sonia.zaibi\@enit.rnu.edu
 * \version 0.8
 * \date first release 
 * \date modification  mohamet.jaafar\@gmail.com 2008
 *
 * This file describe an pgm picture read / write.
 *
 * ----------------------------------------
 * Copyright (C) 2004 Mohamed JAAFAR
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

/* =================================80======================================= */
/*                               INCLUDES                                     */
/* =================================80======================================= */

#include "pgm_reader.h"

#ifdef _USE_MATRIX_TOOLS
#include "matrix_tools.h"
#endif /*_USE_MATRIX_TOOLS*/

/* =================================80======================================= */
/*                               MACRO's                                      */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               TYPEDEF & STRUCTURES & ENUMS                 */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               VARIABLES                                    */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               LOCAL FUNCTIONS                              */
/* =================================80======================================= */

/**
 *
 * \brief   read pgm picture.
 *
 * \param   char *nom_brut                      : path of the pgm picture to read.
 * \param   int **ima_originale                 : output matrix.
 * \param   int *nb_col                         : output number of columns. 
 * \param   int *nb_lin                         : output number of rows.
 * \return  error code
 * \author  sonia.zraibi\@rnu.edu
 * \date    
 */



int lire_image_pgm( char *nom_brut,
                    //int **ima_originale,
                    int *nb_col,
                    int *nb_lin)
{
   int i,j;
   int compteur = 0;
   int *fima;                     //table where pixels are stocked
   char path[PGM_MAX_PATH_LENGTH];
   FILE *pfile_pgm;
   unsigned char *ima_char;
   char char_tampon;
   int ix, iy;
   int fin_entete = 0;
  
  
   /** try to get filename from user **/
   if(nom_brut == NULL)
   {
      printf("\nPlease Give the PGM filename :");
      scanf("%s",nom_brut);
      printf("OK!\n");
   }
   
   /** opening the file **/ 
   if((pfile_pgm=fopen(nom_brut,"rb")) == NULL)
   {
      printf("Erreur d'ouverture n1 : fichier inexistant\n");
      return ERROR_1;
   }
   
   /** the PGM file is already opened **/
   //printf("\nYOUR INPUT FILE : %s\n",nom_brut);
   
   
  /*-----------------------------
    lecture du fichier image  
    -----------------------------*/
  
   /** reading the pgm file header **/
  
  
  /* PGM info :
   * positionnement apres les deux premiers retours chariot
   * !!! attention cela ne suffit plus : en fait il est
   * possible de mettre autant de lignes de commentaires
   * que l'on souhaite, precedees d'un "#".. 
   * en tete : type de fichier PGM 
   *
   */
  
   fscanf(pfile_pgm, "P5\n");
   fin_entete=0;
   
   do
   {
      /** on trouve une nlle ligne de commentaires **/
       if(fgetc(pfile_pgm)=='#')
       do;
       while ( (fgetc(pfile_pgm)!=10) && !feof(pfile_pgm) );    
       else fin_entete=1;
   } 
   while(!fin_entete);
  
   fseek(pfile_pgm, -1, 1);
  
   /** lecture des dimensions **/
   if(feof(pfile_pgm)) 
   {
      printf("erreur : fichier invalide \n");
      return ERROR_2;
   }
   else
   {
      if(fscanf( pfile_pgm, "%d %d\n255\n", nb_col, nb_lin) == 0)
      {
         printf(" erreur : fichier invalide\n");
         return ERROR_2;
      }
   } 
  
   /** lecture des points **/   
   /** allocations **/
  
   ima_char=(unsigned char*)malloc( *nb_col * *nb_lin * sizeof(unsigned char) );
   if((fima = (int *)malloc(*nb_col* *nb_lin * sizeof(int)))==NULL)
   {
      printf("Erreur d'allocation memoire\n");
      return ERROR_3;
   }
  
  
   /* lecture */
   if(fread(ima_char, sizeof(unsigned char), (size_t) *nb_col * *nb_lin,
           pfile_pgm) < (*nb_col * *nb_lin))
   {
      printf ("erreur de lecture\n");
      return ERROR_4;
   }
   
   //printf("end of the image function");
   /* conversion char->int */
   
   int **ima_original = NULL;
#ifdef _USE_MATRIX_TOOLS
   MTOOLS_matrixAllocInt_f(*nb_lin,*nb_col, &ima_original);
#endif /*_USE_MATRIX_TOOLS*/
   
   j=0;
   if(ima_original != NULL)
   { 
      for (i=0; i< *nb_col * *nb_lin ; i++)
      {
         j=(i/(*nb_col));
         //printf("%d\n",j);
         *(ima_original+j)=&fima[(*nb_col)*j];
         fima[i]=(int )ima_char[i];
         //printf("fima[%d]%d",i,fima[i]);
      }
   }
   
   //printf("\ni=%d",i);
   
   free(ima_char);
   fclose(pfile_pgm);
                                          
   return((int)ima_original);
}


/**
 *
 * \brief   write pgm picture.
 *
 * \param   char *nom                          : path of the pgm picture to write.
 * \param   int **ima_originale                : input matrix.
 * \param   int nb_col                         : input number of columns. 
 * \param   int nb_lin                         : input number of rows.
 * \return  error code
 * \author  sonia.zraibi\@rnu.edu
 * \date    
 */
 
int ecrire_image_pgm(char *nom,
                     int** ima_in,
                     int taillex,
                     int tailley)
{
   int i,j;
   char name[PGM_MAX_PATH_LENGTH];
   FILE *pfile;
   unsigned char *ima_inc;
  
   if(nom == 0)
   {
      printf("Nom d'image pgm a ecrire (- si rien): ");
      scanf("%s",name);
      if (name[0]=='-')
      return OK; // No file to write
   }
   else
   {
      sprintf(name,"%s", nom);
   }
   
   /* initialisation du tableau temporaire de char */   
   ima_inc = (unsigned char*)malloc(taillex*tailley*sizeof(unsigned char));
  
   /* conversion des valeurs en uchar */
  
  /* for (i=0 ; i< taillex * tailley ; i++)
   ima_inc[i]=ima_in[i];*/
   
  
   for (i=0 ; i< taillex ; i++)
   for (j=0 ; j< tailley ; j++)
   {
      ima_inc [ taillex *i + j]=ima_in[i][j];
   }
  
   if((pfile=fopen(name,"wb")) == NULL)
   {
      printf("Erreur sauvegarde : impossible d'ouvrir le fichier \n");
      return ERROR_1;
   }
  
   /* ecriture de l'entete */
   fprintf(pfile, "P5\n# file name %s\n%d %d\n255\n",
           name, taillex, tailley);    
  
   /* ecriture des pixels */
  
   if(fwrite(ima_inc, sizeof(unsigned char), (size_t) taillex*tailley,
      pfile)!=taillex*tailley)
   {
      printf ("Erreur de sauvegarde\n");
      return ERROR_5;
   }
   
   free(ima_inc);
   fclose(pfile);                                         
   return OK;
  
}

/* =================================80======================================= */
/*                                FUNCTIONS                                   */
/* =================================80======================================= */

int PGM_readPicture(char* pathPicture, int *row, int *col)
{
   int **pictureMatrix = NULL;
   
   pictureMatrix = (int**) lire_image_pgm(pathPicture, row, col);
   
   if(pictureMatrix != NULL)
   {
      return ((int)pictureMatrix);
   }
   else
   {
      printf("\n[PGM READER] COULD NOT READ PICTURE\n");
   }
}

int PGM_writePicture(char* pathPicture, int **pictureMatrix, int row, int col)
{
   int ret; 
   
   
   if(pictureMatrix != NULL)
   {
      ret = ecrire_image_pgm(pathPicture, pictureMatrix, row, col);
   }
   else
   {
      printf("\n[PGM] NULL matrix pointer\n");
      ret = ERROR_6;
   }
   
   if(ret == OK)
   {
      printf("\nPGM PICTURE GENERATED !!!\n");
   }
   else
   {
      printf("\nFAILED TO GENERATE PGM PICTURE\n" );
   }
   
   return ret;
} 
