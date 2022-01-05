/*
 * UC7101 - Interface with UC7101 (or compatible) LCDs.
 *
 * Copyright (c) 2014 Rustem Iskuzhin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "UC1701.h"

#include <Arduino.h>

//#include <avr/pgmspace.h>


/*
 * If this was a ".h", it would get added to sketches when using
 * the "Sketch -> Import Library..." menu on the Arduino IDE...
 */
#include "charset.cpp"


UC1701::UC1701(unsigned char sclk, unsigned char sid,
                 unsigned char cs1,
                 unsigned char a0,unsigned char rst):
    pin_sclk(sclk),
    pin_sid(sid),
    pin_cs1(cs1),
    pin_a0(a0),
    pin_rst(rst)
{}




void UC1701::begin()
{
    this->width = 96;
    this->height = 49;

    this->column = 0;
    this->line = 0;
    // All pins are outputs 
    pinMode(this->pin_cs1, OUTPUT);
    pinMode(this->pin_a0, OUTPUT);
    pinMode(this->pin_sclk, OUTPUT);
    pinMode(this->pin_sid, OUTPUT);
    pinMode(this->pin_rst, OUTPUT);

    //init the lcd
    digitalWrite(this->pin_rst,0);
    delayMicroseconds(3);
    digitalWrite(this->pin_rst,1);
    delayMicroseconds(6);
    delayMicroseconds(100);//delay 100ms

    // Set the LCD parameters...
    this->Transfer_command(0xE2);  //System Reset
    this->Transfer_command(0x2c);    //set power control 
    this->Transfer_command(0x2e);    //set power control 
    this->Transfer_command(0x2f);    //set power control      
    this->Transfer_command(0x25);    //set vlcd registor radio
    this->Transfer_command(0x81);     //
    this->Transfer_command(0x1a);
    this->Transfer_command(0xa2);     
    this->Transfer_command(0xc8);    //MY=1
    this->Transfer_command(0xa0);     // MX=0
    this->Transfer_command(0xaf);     //
   //digitalWrite(this->pin_cs1, LOW);
    this->clear();

}

void UC1701::clear()
{
    for  (unsigned short j = 0; j < 9; j++) 
    {
        //this->setCursor(0, j);
        this->Transfer_command(0xb0+j);
        this->Transfer_command(0x10);
        this->Transfer_command(0x00);

        for (unsigned short i = 0; i < 132 ; i++) {
            this->Transfer_data(0x00);
        }
    }
    this->setCursor(0, 0);
}


void UC1701::clearRow(int row, int len)
{
    for(unsigned short j = 0; j < len; j++)
    {
        //this->setCursor(0, j);
        this->Transfer_command(0xb0+row+j);
        this->Transfer_command(0x10);
        this->Transfer_command(0x00);

        for (unsigned short i = 0; i < 132 ; i++) {
            this->Transfer_data(0x00);
        }
    }
    this->setCursor(0, 0);
}


void UC1701::setCursor(unsigned char column, unsigned char line)
{
       int msb, lsb;
       column = column+4;
       this->column = column;
       this->line = line;

       msb=(column&0xF0)>>4;
       lsb=column&0x0F;
       digitalWrite(this->pin_cs1, LOW);
       this->Transfer_command(0xb0+line); 
       this->Transfer_command(0x10+msb); //MSB
       this->Transfer_command(0X00+lsb); //LSB
}

void UC1701::writeString(char *msg)
{
    for(int i =0 ; i < strlen(msg); i++)
    {
        this->write(msg[i]);
    }

}

void UC1701::writePosString(char *msg,int col,int row)
{
    this->setCursor(col,row);
    for(int i =0 ; i < strlen(msg); i++)
    {
        this->write(msg[i]);
    }

}


void UC1701::writeBuffer(unsigned char *buf, int len)
{
    int i =0 ;
    for(; i < len ; i++)
    {
        this->Transfer_data(buf[i]);
    }
}

void UC1701::writeBigString(const char *msg,int col,int row)
{
    int line = row;
    int col_max=16,row_max=4,start_pos = col;
    int k,j;
    //this->setCursor(col,line);
    for(int i =0 ; i < strlen(msg); i++)
    {
        char big_chr;
        unsigned char *font_st;
        big_chr = (unsigned char)msg[i];
        big_chr -= 32; // unprintable chars
        font_st = (unsigned char *)&ucBigFont[big_chr*64];
        //16 bytes line
        for(k = 0 ; k < row_max ; k++)
        {
            this->setCursor(start_pos,line+k);
            this->writeBuffer(font_st+k*col_max,col_max);
        }
        start_pos += 16;
    }
}

size_t UC1701::write(uint8_t chr)
{
    // ASCII 7-bit only...
    if (chr >= 0x80) {
        return 0;
    }

    if (chr == '\r') {
        this->setCursor(0, this->line);
        return 1;
    } else if (chr == '\n') {
        this->setCursor(this->column, this->line + 1);
        return 1;
    }

    const unsigned char *glyph;
    unsigned char pgm_buffer[5];

    if (chr >= ' ') {
        // Regular ASCII characters are kept in flash to save RAM...
        memcpy_P(pgm_buffer, &charset[chr - ' '], sizeof(pgm_buffer));
        glyph = pgm_buffer;
    } else {
        // Custom glyphs, on the other hand, are stored in RAM...
        if (this->custom[chr]) {
            glyph = this->custom[chr];
        } else {
            // Default to a space character if unset...
            memcpy_P(pgm_buffer, &charset[0], sizeof(pgm_buffer));
            glyph = pgm_buffer;
        }
    }

    // Output one column at a time...
    for (unsigned char i = 0; i < 5; i++) {
        this->Transfer_data(glyph[i]);
    }

    // One column between characters...
   this->Transfer_data( 0x00);

    // Update the cursor position...
    this->column = (this->column + 6) % this->width;

    if (this->column == 0) {
        this->line = (this->line + 1) % (this->height/9 + 1);
    }

    return 1;
}

void UC1701::createChar(unsigned char chr, const unsigned char *glyph)
{
    // ASCII 0-31 only...
    if (chr >= ' ') {
        return;
    }
    
    this->custom[chr] = glyph;
}

void UC1701::clearLine()
{
    this->setCursor(0, this->line);

    for (unsigned char i = 4; i < 132; i++) {
        this->Transfer_data( 0x00);
    }

    this->setCursor(0, this->line);
}

void UC1701::home()
{
    this->setCursor(0, this->line);
}

void UC1701::drawBitmap(const unsigned char *data, unsigned char columns, unsigned char lines)
{
    unsigned char scolumn = this->column;
    unsigned char sline = this->line;

    // The bitmap will be clipped at the right/bottom edge of the display...
    unsigned char mx = (scolumn + columns > this->width) ? (this->width - scolumn) : columns;
    unsigned char my = (sline + lines > this->height/8) ? (this->height/8 - sline) : lines;

    for (unsigned char y = 0; y < my; y++) {
        this->setCursor(scolumn, sline + y);

        for (unsigned char x = 0; x < mx; x++) {
            this->Transfer_data(data[y * columns + x]);
        }
    }

    // Leave the cursor in a consistent position...
    this->setCursor(scolumn + columns, sline);
}

void UC1701::drawColumn(unsigned char lines, unsigned char value)
{
    unsigned char scolumn = this->column;
    unsigned char sline = this->line;

    // Keep "value" within range...
    if (value > lines*8) {
        value = lines*8;
    }

    // Find the line where "value" resides...
    unsigned char mark = (lines*8 - 1 - value)/8;
    
    // Clear the lines above the mark...
    for (unsigned char line = 0; line < mark; line++) {
        this->setCursor(scolumn, sline + line);
        this->Transfer_data( 0x00);
    }

    // Compute the byte to draw at the "mark" line...
    unsigned char b = 0xff;
    for (unsigned char i = 0; i < lines*8 - mark*8 - value; i++) {
        b <<= 1;
    }

    this->setCursor(scolumn, sline + mark);
    this->Transfer_data(b);

    // Fill the lines below the mark...
    for (unsigned char line = mark + 1; line < lines; line++) {
        this->setCursor(scolumn, sline + line);
        this->Transfer_data(0xff);
    }
  
    // Leave the cursor in a consistent position...
    this->setCursor(scolumn + 1, sline); 
}

void UC1701::Transfer_command(int data1)
{
   char i;
   digitalWrite(this->pin_cs1, LOW);
   digitalWrite(this->pin_a0, LOW);
   for (i=0; i<8; i++)
               {
                 digitalWrite(this->pin_sclk, LOW);
                 if(data1&0x80) digitalWrite(this->pin_sid, HIGH);
                 else digitalWrite(this->pin_sid, LOW);
                 delayMicroseconds(2);
                 digitalWrite(this->pin_sclk, HIGH);
                 delayMicroseconds(2);
                 data1=data1<<1;
               }
}

void UC1701::Transfer_data(int data1)
{
   char i;
   digitalWrite(this->pin_cs1, LOW);
   digitalWrite(this->pin_a0, HIGH);
   for (i=0; i<8; i++)
               {
                 digitalWrite(this->pin_sclk, LOW);
                 if(data1&0x80) digitalWrite(this->pin_sid, HIGH);
                 else digitalWrite(this->pin_sid, LOW);
                 delayMicroseconds(2);
                 digitalWrite(this->pin_sclk, HIGH);
                 delayMicroseconds(2);
                 data1=data1<<1;
               }
}

