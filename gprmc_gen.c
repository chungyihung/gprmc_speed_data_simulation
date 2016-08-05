#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GPRMC "GPRMC"
#define filename "gprmc_sim_data.log"
#define NUMBER_DAT 500
#define uint32 unsigned int
#define PI 3.14159265

/* -----------------------------------------------------------------
 * $--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxxxx,x.x,a*hh
 * ($GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>)
 * ---------------------------------------------------------------*/

typedef struct
{
    double  rcv_time;
    char    status;
    double  lat;
    char    lat_direct;
    double  lon;
    char    lon_direct;
    double  sog;
    double  cog;
    uint32  date;
    double  mag_variation;
    char    mag_var_direct;
} gprmc_format;

int checksum(char *string)
{
    int checksum = string[0];
    int i = 1;
    printf("%s\n", string);

    while( *( string + i ) != '\0' )
    {
        checksum ^= *( string + i);
        i++;
    }
    return checksum;
}

int update_time(double *time)
{
    int hour = *time / 10000;
    int min = *time  / 100 - hour * 100;
    int sec = *time  - hour* 10000 - min * 100;
    sec += 1;
    if( sec >= 60 )
        {
        sec = 0;
        min++;
        if( min >= 60 )
            {
            min = 0;
            hour++;
            if(hour >=24)
                {
                hour = 0;
                }
            }
        }
    *time = hour * 10000 + min * 100 + sec;
    return 0;
}

int main()
{
    FILE *fp;
    int i;
    int chksum = 0;
    char tempstr[500];
    char finalstr[500];

    /* Initialize */
    gprmc_format rmc;
    rmc.rcv_time    = 150000;
    rmc.status      = 'A';
    rmc.lat         = 1800;
    rmc.lat_direct  = 'N';
    rmc.lon         = 12000;
    rmc.lon_direct  = 'W';
    rmc.sog         = 0.0;
    rmc.cog         = 90;   //90 degree toward N
    rmc.date        = 80816;//2016/08/08
    rmc.mag_variation   = 0;
    rmc.mag_var_direct  = 'W';

    fp = fopen(filename, "w");
    if( NULL == fp )
        {
            printf("cannot open file %s\n", filename);
            return 0;
        }

    /* Modify speed by polynomial function estimated by curve fitting */
    for( i = 0; i < NUMBER_DAT ; i++ )
        {
        double j = (double)(i-4) / 150;
        update_time(&rmc.rcv_time);

        // The paramenters of polynomial function is calculated by python curve fitting.
        rmc.sog =   -8.94483843 * pow( 10, -13 ) * pow(i,6) +
                     1.20098136 * pow( 10,  -9 ) * pow(i,5) +
                    -5.84518243 * pow( 10,  -7 ) * pow(i,4) +
                     1.24283805 * pow( 10,  -4 ) * pow(i,3) +
                    -1.16129499 * pow( 10,  -2 ) * pow(i,2) +
                     6.91681116 * pow( 10,  -1 ) * pow(i,1) +
                    -3.46522465 * pow( 10,  -2 ) * pow(i,0);


        sprintf(tempstr, "%s,%06.0f,%c,%.5f,%c,%.5f,%c,%.2f,%.1f,%06u,,,A",
                GPRMC,
                rmc.rcv_time,
                rmc.status,
                rmc.lat,
                rmc.lat_direct,
                rmc.lon,
                rmc.lon_direct,
                rmc.sog,
                rmc.cog,
                rmc.date
                );

        //printf("%s\n", tempstr);
        chksum = checksum(tempstr);
        sprintf(finalstr, "$%s*%X", tempstr, chksum);
        printf("%s\n", finalstr);

        fprintf(fp, "%s\r\n",finalstr );
        }

    fclose(fp);
    return 0;
}
