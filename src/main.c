/****************************************************************************
 * main.c:
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define GETCWD _getcwd
#else
#include <unistd.h>
#define GETCWD getcwd
#endif

#include "dctlib.h"
#include "error.h"
#ifdef _USE_PGM
#include "pgm_reader.h"
#endif
#ifdef _USE_MATRIX_TOOLS
#include "matrix_tools.h"
#endif
#ifdef _USE_JOBS
#include "job_tools.h"
#endif
#ifdef _USE_DCT
#include "dct_tools.h"
#endif

#ifdef _USE_JOBS
JOB_msgQueue_ts dctMsgQueue = {
    .trigger = PTHREAD_COND_INITIALIZER,
    .locker = PTHREAD_MUTEX_INITIALIZER
};
JOB_msgQueue_ts iDctMsgQueue = {
    .trigger = PTHREAD_COND_INITIALIZER,
    .locker = PTHREAD_MUTEX_INITIALIZER
};
#endif

int main(int argc, char** argv)
{
    int ret = OK;
    int **pictureMatrix = NULL;
    int col = 0;
    int row = 0;

#ifdef _USE_PGM
    char picturePath[PGM_MAX_PATH_LENGTH];
    if (argc > 1 && argv[1] != NULL) {
        snprintf(picturePath, sizeof(picturePath), "%s", argv[1]);
    } else {
        snprintf(picturePath, sizeof(picturePath), "lenna.pgm");
    }

    pictureMatrix = PGM_readPicture(picturePath, &row, &col);
    if (pictureMatrix == NULL) {
        char cwd[512] = {0};
        GETCWD(cwd, sizeof(cwd));
        fprintf(stderr, "Failed to load PGM file '%s'. Current directory: %s\n", picturePath, cwd);
        return ERROR_1;
    }
#endif

#if defined(_USE_DCT) && defined(_USE_MATRIX_TOOLS)
    int **ffDct = NULL;
    int **iDct = NULL;
    int **zigzag8X8 = NULL;

    MTOOLS_matrixAllocInt_f(row, col, &ffDct);
    MTOOLS_matrixAllocInt_f(row, col, &iDct);

    DCT_data_ts forwardDctTaskData;
    forwardDctTaskData.inputPictureMatrix = pictureMatrix;
    forwardDctTaskData.outputPictureMatrix = ffDct;
    forwardDctTaskData.col = col;
    forwardDctTaskData.row = row;

    DCT_data_ts iDctTaskData;
    iDctTaskData.inputPictureMatrix = ffDct;
    iDctTaskData.outputPictureMatrix = iDct;
    iDctTaskData.col = col;
    iDctTaskData.row = row;

#ifdef _USE_JOBS
    {
        int z_ret;
        pthread_t forwardDctTaskId;
        pthread_t iDctTaskId;
        int taskPriority;
        int blockIndexI;
        int blockIndexJ;

        z_ret = JTOOLS_msgQueueInit(&dctMsgQueue);
        if (z_ret != OK) {
            fprintf(stderr, "Failed to initialize DCT queue: %d\n", z_ret);
            return z_ret;
        }

        taskPriority = DCT_FFDCT_PRIORITY;
        z_ret = JTOOLS_createJob(&forwardDctTaskId, taskPriority, (void*)&DCT_forwardDct_f, (void*)&forwardDctTaskData);
        if (z_ret != OK) {
            fprintf(stderr, "Failed to create DCT job: %d\n", z_ret);
            return z_ret;
        }

        for (blockIndexI = 0; blockIndexI < (row / DCT_8_X_8_BLOCK); ++blockIndexI) {
            for (blockIndexJ = 0; blockIndexJ < (col / DCT_8_X_8_BLOCK); ++blockIndexJ) {
                JTOOLS_msgQueuePush(&dctMsgQueue, (long)DCT_JOB_COMPUTE, (long)blockIndexI, (long)blockIndexJ);
            }
        }
        JTOOLS_msgQueuePush(&dctMsgQueue, (long)JOB_EXIT, 0, 0);
        JTOOLS_exitJob(&forwardDctTaskId);

        z_ret = JTOOLS_msgQueueInit(&iDctMsgQueue);
        if (z_ret != OK) {
            fprintf(stderr, "Failed to initialize IDCT queue: %d\n", z_ret);
            return z_ret;
        }

        taskPriority = DCT_IDCT_PRIORITY;
        z_ret = JTOOLS_createJob(&iDctTaskId, taskPriority, (void*)&DCT_iDct_f, (void*)&iDctTaskData);
        if (z_ret != OK) {
            fprintf(stderr, "Failed to create IDCT job: %d\n", z_ret);
            return z_ret;
        }

        for (blockIndexI = 0; blockIndexI < (row / DCT_8_X_8_BLOCK); ++blockIndexI) {
            for (blockIndexJ = 0; blockIndexJ < (col / DCT_8_X_8_BLOCK); ++blockIndexJ) {
                JTOOLS_msgQueuePush(&iDctMsgQueue, (long)DCT_JOB_COMPUTE, (long)blockIndexI, (long)blockIndexJ);
            }
        }
        JTOOLS_msgQueuePush(&iDctMsgQueue, (long)JOB_EXIT, 0, 0);
        JTOOLS_exitJob(&iDctTaskId);
    }
#else
    DCT_forwardDct_f(&forwardDctTaskData);
    DCT_iDct_f(&iDctTaskData);
#endif

#ifdef _USE_PGM
    MTOOLS_matrixAllocInt_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &zigzag8X8);
    MTOOLS_zigzagMatrixCollector(zigzag8X8, DCT_8_X_8_BLOCK, NULL);
    MTOOLS_matrixShowInt_f(zigzag8X8, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);

    ret = PGM_writePicture("lenna_dct.pgm", forwardDctTaskData.outputPictureMatrix, row, col);
    if (ret != OK) {
        return ret;
    }

    ret = PGM_writePicture("lenna_idct.pgm", iDctTaskData.outputPictureMatrix, row, col);
    if (ret != OK) {
        return ret;
    }
#endif

    MTOOLS_matrixFreeInt_f(zigzag8X8);
#endif

    return OK;
}
