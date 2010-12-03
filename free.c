#include <ft2build.h>
#include <freetype2/freetype/freetype.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

#define START_X 50
#define START_Y 50
#define FONT_SIZE 16
#define WIDTH 800
#define HEIGHT 600

unsigned char image[600*800][3];

FT_Library library;
FT_Face face;
FT_GlyphSlot slot;
FT_Vector pen;

void draw_bitmap( FT_Bitmap*  bitmap,
		  FT_Int      x,
		  FT_Int      y)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;


	for ( i = x, p = 0; i < x_max; i++, p++ )
	{
		for ( j = y, q = 0; j < y_max; j++, q++ )
		{
			if ( i >= WIDTH || j >= HEIGHT )
			  continue;

			image[j*WIDTH+i][0] = bitmap->buffer[q * bitmap->width + p];
			image[j*WIDTH+i][1] = image[j*WIDTH+i][0];
		}
	}
}

void drawtext(wchar_t *text)
{
	setlocale(LC_CTYPE, "zh_TW.UTF-8");

	FT_Init_FreeType( &library );
	FT_New_Face( library, "courier.ttf", 0, &face );
	FT_Set_Char_Size( face, 0, FONT_SIZE * 64,
			  100, 100 );

	slot = face->glyph;

	pen.x = START_X * 64;
	pen.y = ( HEIGHT - START_Y ) * 64;

	FT_Error error = FT_Select_Charmap( face, FT_ENCODING_UNICODE);
	if ( error != 0 ) {
		printf("select font error");
		perror("select font");
		exit(1);
	}

	int num_chars, n;
	num_chars = wcslen( text );

	for ( n = 0; n < num_chars; ++n )
	{
		FT_Set_Transform( face, NULL, &pen );
		FT_Load_Char( face, text[n], FT_LOAD_RENDER );

		draw_bitmap( &slot->bitmap,
			     slot->bitmap_left,
			     HEIGHT - slot->bitmap_top );

		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	// move to new line
	pen.y -= FONT_SIZE *2 * 64;
	pen.x  = 50 * 64;


	FT_Done_Face(face);
	FT_Done_FreeType(library);

	FILE *fp = fopen("mono.raw", "wb");
	fwrite(image, sizeof(image), 1, fp);
	fclose(fp);
}

int main()
{
	drawtext(L"Fuck you @ 2010/11/23 16:08");
	return 0;
}

