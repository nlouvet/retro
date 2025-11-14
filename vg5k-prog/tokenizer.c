// https://videopac.nl/forum/index.php?topic=1713.0
// Tokenizer program for converting BASIC 80 source code to
// tokenized (binary) form data
// by Gert van der Knokke (c)2011

#include <stdio.h>
#include <string.h>
#define DEBUG   false


// my pseudo ram space
unsigned char ramspace[16384];
#define RAMEND 2048

#define MAXTOKENS       91
static char token[MAXTOKENS][10]={"END","FOR","NEXT","DATA","INPUT","DIM","READ","LET",
                        "GOTO","RUN","IF","RESTORE","GOSUB","RETURN","REM",
                        "STOP","ON","LPRINT","DEF","POKE","PRINT","CONT",
                        "LIST","LLIST","CLEAR","CLOAD","CSAVE","TX","GR",
                        "SCREEN","LINE","DISPLAY","STORE","INIT","CURSORX",
                        "CURSORY","SCROLL","PAGE","BRIGHT","SOUND","DELIM",
                        "SETET","SETEG","ET","EG","NEW","TAB(","TO","FN",
                        "SPC(","THEN","NOT","STEP","+","-","*","/","^","AND",
                        "OR",">","=","<","SGN","INT","ABS","USR","FRE","LPOS",
                        "POS","SQR","RND","LOG","EXP","COS","SIN","TAN","ATN",
                        "PEEK","LEN","STR$","VAL","ASC","STICKX","STICKY",
                        "ACTION","KEY","CHR$","LEFT$","RIGHT$","MID$"};

#define BASICOFFSET     0x88C1 
#define TAPEOFFSET      25

// This functions compares all tokens with given string
// and returns a token number or NULL if no token is found
// Since it compares the exact length of the token even tokens
// not seperated by spaces are recognized (from left to right)
int checktoken(char *string)
{
        int t=0,s=0;

        // scan for all tokens
        while (s<MAXTOKENS && !t)
        {
#if DEBUG
                printf("comparing %s with token %s\n",string,token[s]);
#endif
                // no use comparing strings of smaller size
                if (strlen(string)>=strlen(token[s]))
                {
                        // if a token compares to tokenlength characters of the string
                        // a token is positively found and the loop is finished
                        if (!strncmp(token[s],string,strlen(token[s]))) t=s+0x80;
                }
                s++;
        }
        // return token value or NULL if no token found
        return t;
}

// hexdump ramspace to standard output in 'hexdump -C' style
void hexdump(int limit)
{
        char ascbuf[16];
        int n,m;
        int c;

        if (!limit) limit=RAMEND;

        for (n=0; n<limit; n+=16)
        {
                printf("%08x  ",n+TAPEOFFSET);
                for (m=0; m<16; m++)
                {
                        c=ramspace[n+m] & 0xff;
                        printf("%02x ",c & 0xff);
                        // hexdump -C style
                        if (m==7) printf(" ");
                        if (c>31 && c<128) ascbuf[m]=c;else ascbuf[m]='.';
                }
                ascbuf[m]=0;
                printf(" |%s|\n",ascbuf);
        }
}

// the main event
int main(int argc,char *argv[],char *envp[])
{
        FILE *fp;
        char inbuf[256];
        char temp[256];
        int c;
        int quoteflag,ignoreflag;
        int n,m,r,t,pos,startpos;
        char *valid;

        // pointer in my ram space
        pos=0;
       
        // try to open the input file...
        fp=fopen(argv[1],"r");

        // if it worked, then start the process
        if (fp)
        {
                // loop until end of file
                do
                {
                        // first reset offsetpointers and flags
                        n=0;
                        quoteflag=0;
                        ignoreflag=0;

                        // make inbuf a zero length string
                        inbuf[n]=0;
                        // read one line of input
                        valid=fgets(inbuf,255,fp);

                        // if a line is succesfully read
                        if (valid)
                        {
                                // get rid of the 0x0a at the end of the line
                                if (inbuf[strlen(inbuf)-1]==0x0a) inbuf[strlen(inbuf)-1]=0;     

#if DEBUG
                                // show original line
                                printf("%s\n",inbuf);
#endif
                                // now scan for linenumber
                                while (isdigit(inbuf[n])) temp[n]=inbuf[n++];
       
                                // close temporary string
                                temp[n]=0;


                                // read value from string in 'r'
                                sscanf(temp,"%d",&r);
#if DEBUG
                                printf("read linenumber %d from %s\n",r,temp);
#endif
                                // save current position
                                startpos=pos;

                                // reserve 2 bytes for offset
                                ramspace[pos++]=0x00;   
                                ramspace[pos++]=0x00;

                                // store binary linenumber in rambuffer
                                ramspace[pos++]=(r & 0xff);
                                ramspace[pos++]=(r >> 8);

                               
                                // skip spaces after linenumber
                                while (inbuf[n]==0x20) n++;
                                // now proces this line character by character until end of string (NULL)
                                while (inbuf[n])
                                {
                                        t=0; // clear token number
       
                                        //  toggle quoteflag if quotes are found (string start or end)
                                        if (inbuf[n]==0x22) quoteflag=!quoteflag;

                                        // check for tokens if quoteflag and ignoreflag are not set
                                        if (!quoteflag && !ignoreflag)
                                        {
                                                // check for tokens at current offset (n)
                                                t=checktoken(inbuf+n);
                                                if (t)  // we've found a token
                                                {
                                                        // now skip tokenlength characters in inputbuffer
                                                        n+=strlen(token[t & 0x7f]);
                                                        // write token code into output buffer
                                                        ramspace[pos++]=t;
                                                        // set ignoreflag to ignore all (!) tokens
                                                        // until end of line if token was a REM (0x8e)
                                                        if (t==0x8e) ignoreflag=1;
                                                } // end if (t)
                                        } // end if (!quoteflag && !ignoreflag)

                                        // if no token found or in ignore or string mode...
                                        if (!t)
                                        {
                                                // copy literally to ramspace and advance pointer(s)
                                                // convert some special chars to Videopac's equivalent
                                                c=inbuf[n];
                                                switch (c)
                                                {
                                                        case '$':c=0x04; break;
                                                        case '#':c=0x06; break;
                                                        case '^':c=0x7f; break;
                                                        case 0xa3:c=0x03; break;
                                                        case 0xa8:c=0x7e; break;
                                                }
                                               ramspace[pos++]=c;
                                                n++;
                                        } // end if (!t)
                                } // end while (inbuf[n])
                               
                                // write a NULL byte at the end of this tokenized line
                                ramspace[pos++]=0x00;

                                // fill in the current offset in the reserved offset bytes
                                ramspace[startpos]=(pos + BASICOFFSET) & 0xff;
                                ramspace[startpos+1]=(pos + BASICOFFSET) >> 8;
                        } // end if valid
                }
                while (valid);

                // close the file
                fclose(fp);

                // show the result in hex
                hexdump(pos);
        }
        else
        {
                // error...
                printf("could not open %s!\n",argv[1]);
                return 1;
        }

}


