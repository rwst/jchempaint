#include <stdio.h>
#include <ft2build.h>
#include <wchar.h>
#include <locale.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define FT_FACE_FLAG_SCALABLE          ( 1L <<  0 )
#define FT_FACE_FLAG_FIXED_SIZES       ( 1L <<  1 )
#define FT_FACE_FLAG_FIXED_WIDTH       ( 1L <<  2 )
#define FT_FACE_FLAG_SFNT              ( 1L <<  3 )
#define FT_FACE_FLAG_HORIZONTAL        ( 1L <<  4 )
#define FT_FACE_FLAG_VERTICAL          ( 1L <<  5 )
#define FT_FACE_FLAG_KERNING           ( 1L <<  6 )
#define FT_FACE_FLAG_FAST_GLYPHS       ( 1L <<  7 )
#define FT_FACE_FLAG_MULTIPLE_MASTERS  ( 1L <<  8 )
#define FT_FACE_FLAG_GLYPH_NAMES       ( 1L <<  9 )
#define FT_FACE_FLAG_EXTERNAL_STREAM   ( 1L << 10 )
#define FT_FACE_FLAG_HINTER            ( 1L << 11 )
#define FT_FACE_FLAG_CID_KEYED         ( 1L << 12 )
#define FT_FACE_FLAG_TRICKY            ( 1L << 13 )

void usage()
{
	printf("Usage:    ttf2svgpath fontfile.ttf\n");
}

wint_t* read_characters()
{
	setlocale (LC_ALL, "en_US.UTF-8");
	char *filename = "characters.txt";
	FILE *infile;
	infile = fopen (filename, "r, ccs=UTF-8");
	printf ("File orientation: %d\n", fwide (infile,0));
	static wint_t b[16384], c, *p;
	p = b;
	while ((p-b)<sizeof(b)-4 && (c = fgetwc (infile)) != WEOF)
		{ *p++ = c; printf("%04X ", c); }
	if (c == WEOF) printf (" WEOF \n");
	*p++ = WEOF;
	printf("\nRead %ld wint_t chars from characters.txt\n", p-b);
	printf ("File orientation: %d\n", fwide (infile,0));
	return b;
}

void load_ttfont (wint_t *chrnums, char *fname)
{
	FT_Library ftlib;
	int error = FT_Init_FreeType( &ftlib );
	if ( error ) {
		printf("an error occurred during library initialization!\n"); 
	}
	FT_Face face;
	error = FT_New_Face( ftlib, fname, 0, &face );
	if ( error == FT_Err_Unknown_File_Format )
	{ printf("the font file could be opened and read, but it appears ... that its font format is unsupported\n"); }
	else if ( error ) { printf("the font file could not be opened or read, or it is broken...\n"); }

	printf("Number of glyphs in font: %ld\n", face->num_glyphs);
	printf("This font is scalable: %s\n",
		face->face_flags&FT_FACE_FLAG_SCALABLE? "yes":"no");
	printf("This face uses 'sfnt' storage: %s\n",
		face->face_flags&FT_FACE_FLAG_SFNT? "yes":"no");
	printf("This face contains kerning: %s\n",
		face->face_flags&FT_FACE_FLAG_KERNING? "yes":"no");
	printf("This font contains glyph names: %s\n",
		face->face_flags&FT_FACE_FLAG_GLYPH_NAMES? "yes":"no");
	printf("This font is CID-keyed: %s\n\n",
		face->face_flags&FT_FACE_FLAG_CID_KEYED? "yes":"no");

	printf("Units per EM: %d\n", face->units_per_EM);
	printf("Current height is: %ld\n", face->glyph->metrics.height);
	
	int k=-1;
	while (chrnums[++k] > 0)
	{
		if (chrnums[k] == WEOF) break;
		if (chrnums[k] == 10) continue;
		FT_UInt i = FT_Get_Char_Index (face, chrnums[k]);
		if (i == 0) { printf("FT_Get_Char_Index() returned 0\n"); exit(1); }
		char name[128];
		error = FT_Get_Glyph_Name (face, i, name, 127);
		printf ("%04d: %04d-'%s' ", chrnums[k], i, name);
	
		FT_Glyph glyph;
		int flags = FT_LOAD_NO_SCALE;
		error = FT_Load_Glyph( face, i, FT_LOAD_NO_SCALE );
		if ( error ) { printf("Couldn't FT_Load_glyph()\n"); exit(1); }
		error = FT_Get_Glyph( face->glyph, &glyph );
		if ( error ) { printf("Couldn't FT_Get_glyph()\n"); exit(1); }

		FT_OutlineGlyph *ogp;
		if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
			ogp = &glyph;
		FT_Outline ol = (*ogp)->outline;
		FT_BBox bbox;
		FT_Outline_Get_CBox (&ol, &bbox);
		printf("(%ld, %ld, %ld, %ld) ",
			bbox.xMin, bbox.xMax,
			bbox.yMin, bbox.yMax);
	
		FT_Pos adv = face->glyph->metrics.horiAdvance;
		printf("%ld\n", adv);
	}
}

void load_svgfont (wint_t* wchars, char *fname)
{
}

void write_data (wint_t* wchars)
{
}

int main(int argc, char* argv[])
{
	if (argc != 2) { usage(); return 1; }

	wint_t* wchars = read_characters();
	load_ttfont (wchars, argv[1]);
	load_svgfont (wchars, argv[1]);
	write_data (wchars);
}
