/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *   $Id$    
 *
 *   Copyright (C) 1997 University of Chicago. 
 *   See COPYRIGHT notice in top-level directory.
 */

#include "adio.h"
#include "mpio.h"


#if defined(MPIO_BUILD_PROFILING) || defined(HAVE_WEAK_SYMBOLS)

#if defined(HAVE_WEAK_SYMBOLS)
#if defined(HAVE_PRAGMA_WEAK)
#if defined(FORTRANCAPS)
#pragma weak MPI_FILE_WRITE_AT_ALL_END = PMPI_FILE_WRITE_AT_ALL_END
#elif defined(FORTRANDOUBLEUNDERSCORE)
#pragma weak mpi_file_write_at_all_end__ = pmpi_file_write_at_all_end__
#elif !defined(FORTRANUNDERSCORE)
#pragma weak mpi_file_write_at_all_end = pmpi_file_write_at_all_end
#else
#pragma weak mpi_file_write_at_all_end_ = pmpi_file_write_at_all_end_
#endif

#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#if defined(FORTRANCAPS)
#pragma _HP_SECONDARY_DEF PMPI_FILE_WRITE_AT_ALL_END MPI_FILE_WRITE_AT_ALL_END
#elif defined(FORTRANDOUBLEUNDERSCORE)
#pragma _HP_SECONDARY_DEF pmpi_file_write_at_all_end__ mpi_file_write_at_all_end__
#elif !defined(FORTRANUNDERSCORE)
#pragma _HP_SECONDARY_DEF pmpi_file_write_at_all_end mpi_file_write_at_all_end
#else
#pragma _HP_SECONDARY_DEF pmpi_file_write_at_all_end_ mpi_file_write_at_all_end_
#endif

#elif defined(HAVE_PRAGMA_CRI_DUP)
#if defined(FORTRANCAPS)
#pragma _CRI duplicate MPI_FILE_WRITE_AT_ALL_END as PMPI_FILE_WRITE_AT_ALL_END
#elif defined(FORTRANDOUBLEUNDERSCORE)
#pragma _CRI duplicate mpi_file_write_at_all_end__ as pmpi_file_write_at_all_end__
#elif !defined(FORTRANUNDERSCORE)
#pragma _CRI duplicate mpi_file_write_at_all_end as pmpi_file_write_at_all_end
#else
#pragma _CRI duplicate mpi_file_write_at_all_end_ as pmpi_file_write_at_all_end_
#endif

/* end of weak pragmas */
#endif
/* Include mapping from MPI->PMPI */
#include "mpioprof.h"
#endif

#ifdef FORTRANCAPS
#define mpi_file_write_at_all_end_ PMPI_FILE_WRITE_AT_ALL_END
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpi_file_write_at_all_end_ pmpi_file_write_at_all_end__
#elif !defined(FORTRANUNDERSCORE)
#if defined(HPUX) || defined(SPPUX)
#pragma _HP_SECONDARY_DEF pmpi_file_write_at_all_end pmpi_file_write_at_all_end_
#endif
#define mpi_file_write_at_all_end_ pmpi_file_write_at_all_end
#else
#if defined(HPUX) || defined(SPPUX)
#pragma _HP_SECONDARY_DEF pmpi_file_write_at_all_end_ pmpi_file_write_at_all_end
#endif
#define mpi_file_write_at_all_end_ pmpi_file_write_at_all_end_
#endif

#else

#ifdef FORTRANCAPS
#define mpi_file_write_at_all_end_ MPI_FILE_WRITE_AT_ALL_END
#elif defined(FORTRANDOUBLEUNDERSCORE)
#define mpi_file_write_at_all_end_ mpi_file_write_at_all_end__
#elif !defined(FORTRANUNDERSCORE)
#if defined(HPUX) || defined(SPPUX)
#pragma _HP_SECONDARY_DEF mpi_file_write_at_all_end mpi_file_write_at_all_end_
#endif
#define mpi_file_write_at_all_end_ mpi_file_write_at_all_end
#else
#if defined(HPUX) || defined(SPPUX)
#pragma _HP_SECONDARY_DEF mpi_file_write_at_all_end_ mpi_file_write_at_all_end
#endif
#endif
#endif

/* Prototype to keep compiler happy */
FORTRAN_API void FORT_CALL mpi_file_write_at_all_end_(MPI_Fint *fh,void *buf,MPI_Status *status, 
				int *ierr );

FORTRAN_API void FORT_CALL mpi_file_write_at_all_end_(MPI_Fint *fh,void *buf,MPI_Status *status, int *ierr )
{
    MPI_File fh_c;
    
    fh_c = MPI_File_f2c(*fh);

    *ierr = MPI_File_write_at_all_end(fh_c,buf,status);
}
