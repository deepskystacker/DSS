/*
//
//  Little cms sources filter utility
//  Copyright (C) 1998-2003 Marti Maria
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// A simple filter to strip CR and ^Z, and replace //-like comments to old style

*/

#include <stdio.h>
#include <io.h>


static int ch;
static int IsEof;
static FILE *In, *Out;


static
void NextCh(void)
{
	if (IsEof)
   	ch = 0;
   else {
   		do {

         	ch = getc(In);

            } while (ch == '\r' || ch == '\032');  /* Ignore CR and ^Z */

         IsEof = (ch == EOF);
   		if (IsEof) ch = 0;
   }
}


static
void Translate(void)
{
         IsEof = 0;
         while (!IsEof) {

	         NextCh();
            switch (ch) {

            case '/':
                  NextCh();
                  if (ch == '/') {  /* Found comment */

                        NextCh();

                         /* Cleanup white spaces */
                        while (ch == ' ' && !IsEof)
                              NextCh();

                        if (ch == '\n' && !IsEof) /* Comment is empty */
                              break;
                        
                         /* Comment contains something */

                         putc('/', Out);
                         putc('*', Out);
                         putc(' ', Out);

                         while (ch != '\n' && !IsEof) {

                                  putc(ch, Out);
                                  NextCh();
                         }
                         putc(' ', Out);
                         putc('*', Out);
                         putc('/', Out);
                  }
                  else
                  	putc('/', Out);
                  break;


            default:;
            }

            if (ch != 0)
            		putc(ch, Out);
         }
}


int main(int argc, char *argv[])
{

         if (argc != 3)
         {
              fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
              return 1;

         }

         if (access(argv[2], 0) == 0)
         {
              fprintf(stderr, "%s already exist, please erase manually\n", argv[2]);
              return 1;
         }

         In = fopen(argv[1], "rb");
         if (!In) { perror(argv[1]); return 1;};

         Out = fopen(argv[2], "wb");
         if (!Out) { perror(argv[2]); return 2;};

         Translate();

         fclose(In); fclose(Out);

         return 0;
}

