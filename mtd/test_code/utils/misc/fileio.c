//==========================================================================
//
//      fileio1.c
//
//      Test fileio system
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):           nickg
// Contributors:        nickg
// Date:                2000-05-25
// Purpose:             Test fileio system
// Description:         This test uses the testfs to check out the initialization
//                      and basic operation of the fileio system
//
//
//
//
//
//
//
//####DESCRIPTIONEND####
//
//==========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/times.h>
//==========================================================================
#define true 1
#define false 0
#define SHOW_RESULT( _fn, _res ) \
printf("<FAIL>: " #_fn "() returned %d %s\n", _res, _res<0?strerror(errno):"");

//==========================================================================

#define IOSIZE  100

#define LONGNAME1       "long_file_name_that_should_take_up_more_than_one_directory_entry_1"
#define LONGNAME2       "long_file_name_that_should_take_up_more_than_one_directory_entry_2"

//==========================================================================

#define JFFS2_PART1	"/dev/mtdblock/7"
#define JFFS2_PART2     "/dev/mtdblock/8"

//==========================================================================

#ifndef CYGPKG_LIBC_STRING

char *strcat( char *s1, const char *s2 )
{
    char *s = s1;
    while( *s1 ) s1++;
    while( (*s1++ = *s2++) != 0);
    return s;
}

#endif

//==========================================================================

static void listdir( char *name, int statp, int numexpected, int *numgot )
{
    int err;
    DIR *dirp;
    int num=0;

    printf("<INFO>: reading directory %s\n",name);

    dirp = opendir( name );
    if( dirp == NULL ) SHOW_RESULT( opendir, -1 );

    for(;;)
    {
        struct dirent *entry = readdir( dirp );

        if( entry == NULL )
            break;
        num++;
        printf("<INFO>: entry %14s",entry->d_name);
        if( statp )
        {
            char fullname[PATH_MAX];
            struct stat sbuf;

            if( name[0] )
            {
                strcpy(fullname, name );
                if( !(name[0] == '/' && name[1] == 0 ) )
                    strcat(fullname, "/" );
            }
            else fullname[0] = 0;

            strcat(fullname, entry->d_name );

            err = stat( fullname, &sbuf );
            if( err < 0 )
            {
                if( errno == ENOSYS )
                    printf(" <no status available>");
                else SHOW_RESULT( stat, err );
            }
            else
            {
                printf(" [mode %08x ino %08x nlink %d size %d]",
                            sbuf.st_mode,sbuf.st_ino,sbuf.st_nlink,sbuf.st_size);
            }
        }

        printf("\n");
    }

    err = closedir( dirp );
    if( err < 0 ) SHOW_RESULT( stat, err );
    if (numexpected >= 0 && num != numexpected)
        printf("Wrong number of dir entries\n");
    if ( numgot != NULL )
        *numgot = num;
}

//==========================================================================

static void createfile( char *name, size_t size )
{
    char buf[IOSIZE];
    int fd;
    ssize_t wrote;
    int i;
    int err;

    printf("<INFO>: create file %s size %d\n",name,size);

    err = access( name, F_OK );
    if( err < 0 && errno != EACCES && errno != ENOENT) {
	    printf("access errno is %d\n",errno);
	    SHOW_RESULT( access, err );
    }
    for( i = 0; i < IOSIZE; i++ ) buf[i] = i%256;

    fd = open( name, O_WRONLY|O_CREAT );
    if( fd < 0 ) SHOW_RESULT( open, fd );

    while( size > 0 )
    {
        ssize_t len = size;
        if ( len > IOSIZE ) len = IOSIZE;

        wrote = write( fd, buf, len );
        if( wrote != len ) SHOW_RESULT( write, wrote );

        size -= wrote;
    }

    err = close( fd );
    if( err < 0 ) SHOW_RESULT( close, err );
}

//==========================================================================

static void checkfile( char *name )
{
    char buf[IOSIZE];
    int fd;
    ssize_t done;
    int i;
    int err;
    off_t pos = 0;

    printf("<INFO>: check file %s\n",name);

    err = access( name, F_OK );
    if( err != 0 && errno != ENOENT) {
	    SHOW_RESULT( access, err );
	    printf("access errno is %d\n",errno);
    }
    fd = open( name, O_RDONLY );
    if( fd < 0 ) SHOW_RESULT( open, fd );

    for(;;)
    {
        done = read( fd, buf, IOSIZE );
        if( done < 0 ) SHOW_RESULT( read, done );

        if( done == 0 ) break;

        for( i = 0; i < done; i++ )
            if( buf[i] != i%256 )
            {
                printf("buf[%d+%d](%02x) != %02x\n",pos,i,buf[i],i%256);
                printf("Data read not equal to data written\n");
            }

        pos += done;
    }

    err = close( fd );
    if( err < 0 ) SHOW_RESULT( close, err );
}

//==========================================================================

static void copyfile( char *name2, char *name1 )
{

    int err;
    char buf[IOSIZE];
    int fd1, fd2;
    ssize_t done, wrote;

    printf("<INFO>: copy file %s -> %s\n",name2,name1);

    err = access( name1, F_OK );
    if( err < 0 && errno != EACCES && errno != ENOENT) SHOW_RESULT( access, err );

    err = access( name2, F_OK );
    if( err != 0 ) SHOW_RESULT( access, err );

    fd1 = open( name1, O_WRONLY|O_CREAT );
    if( fd1 < 0 ) SHOW_RESULT( open, fd1 );

    fd2 = open( name2, O_RDONLY );
    if( fd2 < 0 ) SHOW_RESULT( open, fd2 );

    for(;;)
    {
        done = read( fd2, buf, IOSIZE );
        if( done < 0 ) SHOW_RESULT( read, done );

        if( done == 0 ) break;

        wrote = write( fd1, buf, done );
        if( wrote != done ) SHOW_RESULT( write, wrote );

        if( wrote != done ) break;
    }

    err = close( fd1 );
    if( err < 0 ) SHOW_RESULT( close, err );

    err = close( fd2 );
    if( err < 0 ) SHOW_RESULT( close, err );

}

//==========================================================================

static void comparefiles( char *name2, char *name1 )
{
    int err;
    char buf1[IOSIZE];
    char buf2[IOSIZE];
    int fd1, fd2;
    ssize_t done1, done2;
    int i;

    printf("<INFO>: compare files %s == %s\n",name2,name1);

    err = access( name1, F_OK );
    if( err != 0 ) SHOW_RESULT( access, err );

    err = access( name1, F_OK );
    if( err != 0 ) SHOW_RESULT( access, err );

    fd1 = open( name1, O_RDONLY );
    if( fd1 < 0 ) SHOW_RESULT( open, fd1 );

    fd2 = open( name2, O_RDONLY );
    if( fd2 < 0 ) SHOW_RESULT( open, fd2 );

    for(;;)
    {
        done1 = read( fd1, buf1, IOSIZE );
        if( done1 < 0 ) SHOW_RESULT( read, done1 );

        done2 = read( fd2, buf2, IOSIZE );
        if( done2 < 0 ) SHOW_RESULT( read, done2 );

        if( done1 != done2 )
            printf("Files different sizes\n");

        if( done1 == 0 ) break;

        for( i = 0; i < done1; i++ )
            if( buf1[i] != buf2[i] )
            {
                printf("buf1[%d](%02x) != buf1[%d](%02x)\n",i,buf1[i],i,buf2[i]);
                printf("Data in files not equal\n");
            }
    }

    err = close( fd1 );
    if( err < 0 ) SHOW_RESULT( close, err );

    err = close( fd2 );
    if( err < 0 ) SHOW_RESULT( close, err );

}

//==========================================================================

void checkcwd( const char *cwd )
{
    static char cwdbuf[PATH_MAX];
    char *ret;

    ret = getcwd( cwdbuf, sizeof(cwdbuf));
    if( ret == NULL ) SHOW_RESULT( getcwd, ret );

    if( strcmp( cwdbuf, cwd ) != 0 )
    {
        printf( "cwdbuf %s cwd %s\n",cwdbuf, cwd );
        printf( "Current directory mismatch");
    }
}

timestest() {

	int fd;
	struct tms buf;
	clock_t old, new;
	long clk;
	int point1sec, point01sec;
	clk = sysconf(_SC_CLK_TCK);
	point1sec = clk/10;
	point01sec = clk/100;
	old = times(&buf);
	fd = open("/jffs2/test1",O_RDWR|O_CREAT,0555);
	new = times(&buf);
	if((new - old) > point1sec)
		SHOW_RESULT(create_new,-1);
	close(fd);
	old = times(&buf);
	fd = open("/jffs2/test1",O_RDONLY);
	new = times(&buf);
	if((new - old) > point01sec)
		SHOW_RESULT(open_existing,-1);
	close(fd);
	unlink("/jffs2/test1");
}

//==========================================================================
// main

int main( int argc, char **argv )
{
    int err;
    int existingdirents=-1;

    char *jffs2_part1, *jffs2_part2;
    int part1_mounted = 0, part2_mounted = 0;

    if (argc > 1)
        if (!strcmp(argv[1],"-h") || !strcmp(argv[1], "--help")){
            printf("fileio <dev1> <dev2>\n");
            printf("perform file system functional test on <dev1> and <dev2>\n");
            exit(0);
        }
    jffs2_part1 = JFFS2_PART1;
    jffs2_part2 = JFFS2_PART2;


    if (argc > 1) jffs2_part1 = argv[1];
    if (argc > 2) jffs2_part2 = argv[2];

    mkdir("/mnt/test",0755);
    mkdir("/jffs2",0755);
    err = mount( jffs2_part1, "/mnt/test", "jffs2",0,NULL );
    if( err < 0 ){
        SHOW_RESULT( mount, err );
    }
    else {
    part1_mounted = 1;
    err = chdir( "/mnt/test" );
    if( err < 0 ) SHOW_RESULT( chdir, err );

    checkcwd( "/mnt/test" );

    listdir( "/mnt/test", true, -1, &existingdirents );
    if ( existingdirents < 2 )
        printf("Not enough dir entries\n");


    createfile( "foo", 202 );
    checkfile( "foo");
    copyfile( "foo", "fee");
    checkfile( "fee" );
    comparefiles( "foo", "fee" );
    printf("<INFO>: mkdir bar\n");
    err = mkdir( "bar", 0 );
    if( err < 0 ) SHOW_RESULT( mkdir, err );

    listdir( "/mnt/test" , true, existingdirents+3, NULL );

    copyfile( "fee", "bar/fum" );
    checkfile( "bar/fum" );
    comparefiles( "fee", "bar/fum" );

    printf("<INFO>: cd /mnt/test/bar\n");
    err = chdir( "/mnt/test/bar" );
    if( err < 0 ) SHOW_RESULT( chdir, err );

    checkcwd( "/mnt/test/bar" );

    printf("<INFO>: rename /foo bundy\n");
    err = rename( "/mnt/test/foo", "bundy" );
    if( err < 0 ) SHOW_RESULT( rename, err );

    listdir( "/mnt/test", true, existingdirents+2, NULL );

    checkfile( "bundy" );
    comparefiles("/mnt/test/fee", "/mnt/test/bar/bundy" );

    // --------------------------------------------------------------

    createfile( LONGNAME1, 123 );
    checkfile( LONGNAME1 );
    copyfile( LONGNAME1, LONGNAME2 );

    printf("<INFO>: unlink " LONGNAME1 "\n");
    err = unlink( LONGNAME1 );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    printf("<INFO>: unlink " LONGNAME2 "\n");
    err = unlink( LONGNAME2 );
    if( err < 0 ) SHOW_RESULT( unlink, err );


    // --------------------------------------------------------------

    printf("<INFO>: unlink fee\n");
    err = unlink( "/mnt/test/fee" );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    printf("<INFO>: unlink fum\n");
    err = unlink( "fum" );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    printf("<INFO>: unlink bundy\n");
    err = unlink( "bundy" );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    printf("<INFO>: cd /\n");
    err = chdir( "/mnt/test" );
    if( err < 0 ) SHOW_RESULT( chdir, err );

    checkcwd( "/mnt/test" );

    printf("<INFO>: rmdir /bar\n");
    err = rmdir( "/mnt/test/bar" );
    if( err < 0 ) SHOW_RESULT( rmdir, err );

    listdir( "/mnt/test", false, existingdirents, NULL );

    }
    // --------------------------------------------------------------

    printf("<INFO>: mount /jffs2 \n");
    err = mount( jffs2_part2, "/jffs2", "jffs2" ,0,NULL);
    if( err < 0 ) {
    SHOW_RESULT( mount, err );
    }
    else {
    part2_mounted = 1;
    createfile( "/jffs2/tinky", 456 );
    copyfile( "/jffs2/tinky", "/jffs2/laalaa" );
    checkfile( "/jffs2/tinky");
    checkfile( "/jffs2/laalaa");
    comparefiles( "/jffs2/tinky", "/jffs2/laalaa" );

    printf("<INFO>: cd /jffs2\n");
    err = chdir( "/jffs2" );
    if( err < 0 ) SHOW_RESULT( chdir, err );

    checkcwd( "/jffs2" );

    printf("<INFO>: mkdir noonoo\n");
    err = mkdir( "noonoo", 0 );
    if( err < 0 ) SHOW_RESULT( mkdir, err );

    listdir( "." , true, existingdirents+3, NULL);

    printf("<INFO>: cd noonoo\n");
    err = chdir( "noonoo" );
    if( err < 0 ) SHOW_RESULT( chdir, err );

    checkcwd( "/jffs2/noonoo" );

    createfile( "tinky", 678024 );
    checkfile( "tinky" );

    createfile( "dipsy", 3456 );
    checkfile( "dipsy" );
    copyfile( "dipsy", "po" );
    checkfile( "po" );
    comparefiles( "dipsy", "po" );


    listdir( ".", true, 5, NULL );
    listdir( "..", true, existingdirents+3, NULL );

    // --------------------------------------------------------------

    printf("<INFO>: unlink tinky\n");
    err = unlink( "tinky" );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    printf("<INFO>: unlink dipsy\n");
    err = unlink( "dipsy" );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    printf("<INFO>: unlink po\n");
    err = unlink( "po" );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    printf("<INFO>: cd ..\n");
    err = chdir( ".." );
    if( err < 0 ) SHOW_RESULT( chdir, err );
    checkcwd( "/jffs2" );

    printf("<INFO>: rmdir noonoo\n");
    err = rmdir( "noonoo" );
    if( err < 0 ) SHOW_RESULT( rmdir, err );

    // --------------------------------------------------------------

    err = mkdir( "x", 0 );
    if( err < 0 ) SHOW_RESULT( mkdir, err );

    err = mkdir( "x/y", 0 );
    if( err < 0 ) SHOW_RESULT( mkdir, err );

    err = mkdir( "x/y/z", 0 );
    if( err < 0 ) SHOW_RESULT( mkdir, err );

    err = mkdir( "x/y/z/w", 0 );
    if( err < 0 ) SHOW_RESULT( mkdir, err );

    printf("<INFO>: cd /jffs2/x/y/z/w\n");
    err = chdir( "/jffs2/x/y/z/w" );
    if( err < 0 ) SHOW_RESULT( chdir, err );
    checkcwd( "/jffs2/x/y/z/w" );

    printf("<INFO>: cd ..\n");
    err = chdir( ".." );
    if( err < 0 ) SHOW_RESULT( chdir, err );
    checkcwd( "/jffs2/x/y/z" );

    printf("<INFO>: cd .\n");
    err = chdir( "." );
    if( err < 0 ) SHOW_RESULT( chdir, err );
    checkcwd( "/jffs2/x/y/z" );

    printf("<INFO>: cd ../../y\n");
    err = chdir( "../../y" );
    if( err < 0 ) SHOW_RESULT( chdir, err );
    checkcwd( "/jffs2/x/y" );

    printf("<INFO>: cd ../..\n");
    err = chdir( "../.." );
    if( err < 0 ) SHOW_RESULT( chdir, err );
    checkcwd( "/jffs2" );

    printf("<INFO>: rmdir x/y/z/w\n");
    err = rmdir( "x/y/z/w" );
    if( err < 0 ) SHOW_RESULT( rmdir, err );

    printf("<INFO>: rmdir x/y/z\n");
    err = rmdir( "x/y/z" );
    if( err < 0 ) SHOW_RESULT( rmdir, err );

    printf("<INFO>: rmdir x/y\n");
    err = rmdir( "x/y" );
    if( err < 0 ) SHOW_RESULT( rmdir, err );

    printf("<INFO>: rmdir x\n");
    err = rmdir( "x" );
    if( err < 0 ) SHOW_RESULT( rmdir, err );

    // --------------------------------------------------------------

    printf("<INFO>: unlink tinky\n");
    err = unlink( "tinky" );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    printf("<INFO>: unlink laalaa\n");
    err = unlink( "laalaa" );
    if( err < 0 ) SHOW_RESULT( unlink, err );
    }

    printf("<INFO>: cd /\n");
    err = chdir( "/" );
    if( err < 0 ) SHOW_RESULT( chdir, err );
    checkcwd( "/" );
    timestest();

    if (part2_mounted){
    printf("<INFO>: umount /jffs2\n");
    err = umount( "/jffs2" );
    if( err < 0 ) SHOW_RESULT( umount, err );
    }

    if (part1_mounted) {
    printf("<INFO>: umount /mnt/test\n");
    err = umount( "/mnt/test" );
    if( err < 0 ) SHOW_RESULT( umount, err );
    }

}

// -------------------------------------------------------------------------
// EOF fileio1.c
