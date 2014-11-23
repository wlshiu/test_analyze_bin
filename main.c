#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

#include "parse_json.h"
#include "pthread.h"

//////////////////////////////////////////////////////////
static char     *pJson_path = 0;
static char     *pIn_path = 0;
static char     *pOut_path = 0;

//////////////////////////////////////////////////////////
static void
usage(void)
{
    static char str[] =
        "Usage: exe [options] \n"
        "options:\n";

    printf("%s", str);
    return;
}

static void
_getparams(int argc, char **argv)
{
    if( argc < 2 )
    {
        usage();
        while(1);
    }

    argv++; argc--;
    while(argc)
    {
        if( !strcmp(argv[0], "--path-json") )
        {
            pJson_path = argv[0];
        }
        else if( !strcmp(argv[0], "--path-input") )
        {
            pIn_path = argv[0];
        }
        else if( !strcmp(argv[0], "--path-output") )
        {
            pOut_path = argv[0];
        }
        else
        {

        }
        argv++; argc--;
    }
    return;
}

static void
_test_parse_json(void)
{
    Parse_Json(pJson_path, 0);
    return;
}

//////////////////////////////////////////////////////////
int
main(int argc, char **argv)
{
#ifdef PTW32_STATIC_LIB
    pthread_win32_process_attach_np();
#endif

    _getparams(argc, argv);

    _test_parse_json();

#ifdef PTW32_STATIC_LIB
    pthread_win32_process_detach_np();
#endif
    return 0;
}