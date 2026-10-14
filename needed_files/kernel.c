/* 
Name: Mohammed Miahjee

Changes 
Date    Edits
====    ==================
10/13   Added carriage return to readString method to avoid printing directly below.


*/

/*prototypes*/
printString(char*);               /*interrupt 0x10  */
readString(char*);            /*interrupt 0x16  */
readSector(char*, int);
void handleInterrupt21(int, int, int, int);

main() {

        char a[80];
	char* msg = "Hello World";
	char buffer[512];
	char* msg3 = "\r\n\nHello World!\r\nThis message was called from the int-21 call.\r\n\n";
	int l = 11;			/*length of string*/
	
	int start = 0xB800;		/*starting memory add*/
	int offset = 0x140;		/*offset to third line*/
	int color = 0x7;		/*color - white*/
	
	int i;
	for (i=0; i<l;i++) {
	
		putInMemory(start, offset, msg[i]);	/*Load next char to memory*/
		putInMemory(start, offset+1, color);	/*Load color to memory*/		

		offset += 2;				/*2 bytes/ character*/

	}
	    
	
	readSector(buffer, 30);          /*Read from sector 30 and save to buffer  */
        printString(buffer);            /*Print buffer  */
        
        makeInterrupt21();              /*Ties custom function to Int 21 call.  */
        
        //0-printString, 1-readString, 2-readSector */
        interrupt(0x21,0,msg3,0,0);     //Print msg3  */
        interrupt(0x21,0,"\r\nEnter a line: ",0,0);     /*Print */
        interrupt(0x21,1,a,0,0);     /*Read input - echo */
        interrupt(0x21,0,"\r\nYou entered: ",0,0);
        interrupt(0x21,0,a,0,0);     /*print input from last Int_21 call*/

	while(1);					/*Infinite loop*/

}


/*--------------------------------------*/

/*Print to screen */
printString(char* chars)          
{
  int i = 0;                    /*index*/
  
  while(chars[i] != 0x0)        //Stop at 0x0
  {
    char al = chars[i];                         //al = char at i
    interrupt(0x10, 0xe*256+al, 0, 0, 0);       //interrupt 0x10
    
    i += 1;
  }
}

/*--------------------------------------*/
/*Read from input and print to screen */
readString(char* msg)
{
  int i = 0;
  char a;
  while(1)                        /*Loop indefinitely */
  {
    a = interrupt(0x16, 0, 0, 0, 0);   /*call interrupt 0x16 */
    if (a == 0xd)                           /*Stop when 0xd (enter) is pressed  */
    {
      msg[i] = 0xa;                         /*Add \n + \r + 0x0 at the end */
      msg[i+1] = '\r';  
      msg[i+2] = 0x0;
      
      interrupt(0x10, 0xe*256+msg[i], 0, 0, 0);   /*print line feed */  
      interrupt(0x10, 0xe*256+msg[i+1], 0, 0, 0);   /*print carriage return */
 
      break;
    }
    msg[i] = a;        /*add the entry to the array and increment */
    interrupt(0x10, 0xe*256+a, 0, 0, 0);      //print as the user types
    i += 1;
  }
  
}

/*Read from a specific sector and save to a variable- interrupt 0x13  */
readSector(char* buffer, int sector)
{

  int ah = 2;      /*BIOS to read from a sector  */
  int al = 1;       /*# of sectors to read  */
  int bx = buffer;   /*Store read data to  */
  int ch = 0;       /*track */
  int cl = sector+1;  /*Relative sector (sector+1)  */
  int dh = 0;       /*head  */
  int dl = 0x80;     /*device number - 0x80 = hard disk  */
  
  int ax = ah*256+al; int cx = ch*256+cl; int dx = dh*256+dl;
  

  /* Int 0x13 - read from sector 30 -> buffer (bx)  */
  interrupt(0x13, ax, buffer, cx, dx);        

}

/*Custom function to tie to interrupt 0x21  */
void handleInterrupt21(int ax, int bx, int cx, int dx)
{
  switch (ax) {   /*0-printString, 1-readString, 2-readSector */
    case 0:
      printString(bx);   /*bx = address of char array to print  */
      break;
    
    case 1:
      readString(bx);     /*bx = Char array to save user input  */
      break;
  
    case 2:
      readSector(bx,cx);  /*bx = save to, sector to read from */
      break;
    
    default:
      printString("\r\nInvalid option selected.");
      break;
  }
}









