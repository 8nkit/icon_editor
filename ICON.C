/* Refer Appendix B in the book if you want to create standalone
   EXE file of this program during compilation */

# include "stdio.h"
# include "dos.h"
# include "bios.h"
# include "graphics.h"
# include "process.h"

# define MAXX 40  /* maximum x-coordinate of icon */
# define MAXY 32  /* maximum y-coordinate of icon */
# define DRAW 0
# define SKIP 1
# define ERASE 2
# define EXP_FACTOR 4  /* expansion factor for grid */
# define BACK_GND_GRID 1  /* background color for grid */
# define BACK_GND_ICON 0  /* background color for icon */
# define FORE_GND 3  /* foreground color for grid and icon */

/* array to store image of the icon */
unsigned char icon_image [MAXX][MAXY] ;

int scan ;

main ( int argc, char *argv[] )
{
	int gm = CGAC3, gd = CGA ;
	char fname[30] ;

	/* initialise graphics system in medium resolution graphics mode */
	initgraph ( &gd, &gm, "c:\\tc\\bgi" ) ;

	/* if icon to be loaded is mentioned at DOS prompt */
	if ( argc == 2 )
		load_icon ( argv[1] ) ;
	else
	{
		/* initialise icon image array */
		init_icon() ;
	}

	/* display the icon in the icon box */
	display_icon() ;

	/* display the icon in the grid */
	display_grid() ;

	while ( 1 )
	{
		/* display main menu */
		gotoxy ( 2, 25 ) ;
		printf ( "F1-Draw   F2-Load   F3-Save   F4-Quit" ) ;

		/* get user's choice */
		getkey() ;

		switch ( scan )
		{
			case 59 :  /* F1 key */

				edit_icon() ;
				break ;

			case 60 :  /* F2 key */

				/* collect the file name */
				gotoxy ( 6, 21 ) ;
				printf ( "Enter file name: " ) ;
				gets ( fname ) ;

				/* erase the message line */
				cleartext ( 21 ) ;

				load_icon ( fname ) ;
				display_icon() ;
				display_grid() ;
				break ;

			case 61 :  /* F3 key */

				save_icon() ;
				break ;

			case 62 :  /* F4 key */

				/* restore original video mode */
				closegraph() ;
				restorecrtmode() ;

				exit ( 0 ) ;
		}
	}
}

/* reads icon from file into memory */
load_icon ( char *name )
{
	FILE *fp ;

	/* open file and check if successful */
	if ( ( fp = fopen ( name, "rb" ) ) == NULL )
	{
		gotoxy ( 1, 21 ) ;
		printf ( "Unable to open file! Press any key..." ) ;
		getch() ;

		cleartext ( 21 ) ;
		return ;
	}

	fread ( icon_image, sizeof ( icon_image ), 1, fp ) ;
	fclose ( fp ) ;
}

/* erases the message line */
cleartext ( int row )
{
	int col ;

	for ( col = 1 ; col <= 39 ; col++ )
	{
		gotoxy ( col, row ) ;
		printf ( " " ) ;
	}
}

/* initialises the icon image array */
init_icon()
{
	int x, y ;

	for ( x = 0 ; x < MAXX ; x++ )
	{
		for ( y = 0 ; y < MAXY ; y++ )
			icon_image[x][y] = BACK_GND_ICON ;
	}
}

/* displays the current icon */
display_icon()
{
	int x, y ;

	setcolor ( 3 ) ;
	rectangle ( 0, 0, 41, 33 ) ;

	for ( x = 0 ; x < MAXX ; x++ )
	{
		for ( y = 0 ; y < MAXY ; y++ )
			putpixel ( x + 1, y + 1, icon_image[x][y] ) ;
	}
}

/* constructs the grid as per the expansion factor */
display_grid()
{
	int x, y, xgrid, ygrid ;

	for ( x = 0 ; x < MAXX ; x++ )
	{
		for ( y = 0 ; y < MAXY ; y++ )
		{
			xgrid = 100 + x * EXP_FACTOR ;
			ygrid = y * EXP_FACTOR ;

			if ( icon_image[x][y] == 0 )
				putpixel ( xgrid, ygrid, 1 ) ;
			else
				putpixel ( xgrid, ygrid, icon_image[x][y] ) ;
		}
	}
}

/* collects the scan code of the key hit */
getkey()
{
	union REGS i, o ;

	/* wait until a key is hit */
	while ( !kbhit() )
		;

	i.h.ah = 0 ;  /* service number */
	int86 ( 22, &i, &o ) ;  /* issue interrupt */

	scan = o.h.ah ;  /* collect scan code of key hit */
}

/* allows drawing/editing of the icon */
edit_icon()
{
	int x = 0, y = 0, xgrid = 100, ygrid = 0, flag = SKIP ;

	/* display menu */
	gotoxy ( 1, 25 ) ;
	printf ( "F1-Draw F2-Skip F3-Erase F4-Wipe F5-End" ) ;

	while ( 1 )
	{
		/* draw cursor */
		drawcursor ( xgrid, ygrid, FORE_GND ) ;

		getkey() ;  /* receive key */

		/* erase cursor */
		drawcursor ( xgrid, ygrid, BACK_GND_ICON ) ;

		/* display the current pixel in the grid in its previous color */
		if ( icon_image[x][y] == 0 )
			putpixel ( xgrid, ygrid, BACK_GND_GRID ) ;
		else
			putpixel ( xgrid, ygrid, icon_image[x][y] ) ;

		switch ( scan )
		{
			case 59 :  /* F1 key */

				flag = DRAW ;
				break ;

			case 60 :  /* F2 key */

				flag = SKIP ;
				break ;

			case 61 :  /* F3 key */

				flag = ERASE ;
				break ;

			case 62 :  /* F4 key */

				init_icon() ;
				display_icon () ;
				display_grid()  ;
				break ;

			case 63 :  /* F5 key */

				cleartext ( 25 ) ;
				return ;

			case 75 :  /* left arrow key */

				x-- ;
				break ;

			case 77 :  /* right arrow key */

				x++ ;
				break ;

			case 72 :  /* up arrow key */

				y-- ;
				break ;

			case 80 :  /* down arrow key */

				y++ ;
				break ;

			case 71 :  /* Home key */

				x-- ;
				y-- ;
				break ;

			case 73 :  /* End key */

				x++ ;
				y-- ;
				break ;

			case 79 :  /* PgUp key */

				x-- ;
				y++ ;
				break ;

			case 81 :  /* PgDn key */

				x++ ;
				y++ ;
				break ;

			default :  /* any other key */

				printf ( "\a" ) ;
		}

		/* readjust x and y if they exceed the limits */

		if ( x < 0 )
			x++ ;

		if ( y < 0 )
			y++;

		if ( x == MAXX )
			x-- ;

		if ( y == MAXY )
			y-- ;

		if ( flag == DRAW )
			icon_image[x][y] = FORE_GND ;

		if ( flag == ERASE )
			icon_image[x][y] = BACK_GND_ICON ;

		/* display the corresponding pixel in the icon box */
		putpixel ( x + 1, y + 1, icon_image[x][y] ) ;

		/* calculate coordinates of corresponding pixel in grid */
		xgrid = 100 + x * EXP_FACTOR ;
		ygrid = y * EXP_FACTOR ;

		/* display the corresponding pixel in grid */
		if ( icon_image[x][y] == 0 )
			putpixel ( xgrid, ygrid, BACK_GND_GRID ) ;
		else
			putpixel ( xgrid, ygrid, icon_image[x][y] ) ;
	}
}

/* saves current icon into file */
save_icon()
{
	FILE *fp ;
	char fname[30] ;

	/* collect the file name */
	gotoxy ( 6, 21 ) ;
	printf ( "Enter the file name: " ) ;
	gets ( fname ) ;

	/* erase the message line */
	cleartext ( 21 ) ;

	/* open file, exit if unsuccessful */
	if ( ( fp = fopen ( fname, "wb" ) ) == NULL )
	{
		gotoxy ( 1, 21 ) ;
		printf ( "Unable to open file! Press any key..." ) ;
		getch() ;

		cleartext ( 21 ) ;
		return ;
	}

	fwrite ( icon_image, sizeof ( icon_image ), 1, fp ) ;
	fclose ( fp ) ;
}

/* draws or erases the graphics cursor */
drawcursor ( int xgrid, int ygrid, int color )
{
	int xx, yy ;

	for ( xx = xgrid - 2 ; xx <= xgrid + 2 ; xx++ )
		putpixel ( xx, ygrid, color ) ;

	for ( yy = ygrid - 2 ; yy <= ygrid + 2 ; yy++ )
		putpixel ( xgrid, yy, color ) ;
}