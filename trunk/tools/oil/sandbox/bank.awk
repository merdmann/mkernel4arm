# ***************************************************************************
# *  $Id: bank.awk 219 2008-10-12 17:23:57Z merdmann $
# *
# *  Thu Nov 29 21:14:09 2007
# *  Copyright  2007, 2008 Joerg Tröger
# *  Email: Jörg Tröger <joerg.troeger@versanet.de>
# ****************************************************************************/

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

BEGIN { bankNum = 0; debug=0; highestCodeAddr = 0; }

function Hex2Dez(hex)
{
  dez = 0;

  while (length(hex)) {
    dez *= 16;
    c = substr(hex, 1, 1);
    hex = substr(hex, 2, length(hex)-1);
    if ((c == "a") || (c == "A")) dez += 10;
    else if ((c == "b") || (c == "B")) dez += 11;
    else if ((c == "c") || (c == "C")) dez += 12;
    else if ((c == "d") || (c == "D")) dez += 13;
    else if ((c == "e") || (c == "E")) dez += 14;
    else if ((c == "f") || (c == "F")) dez += 15;
    else dez += 0+c;
  }

  return(dez);
}

/code/ {
   if (($2 == "code") && ($1 != ".config")){
     thisCodeAddr = 0+Hex2Dez(substr($3, 3, length($3)-2));
     if (thisCodeAddr > highestCodeAddr) {
       highestCodeAddr = thisCodeAddr;
     }
   }
}

/DATABANK/ {
  if ($1 == "DATABANK") {
    name = substr($2, 6, length($2)-5);
    s = 0+Hex2Dez(substr($3, 7, length($3)-6));
    e   = 1+Hex2Dez(substr($4, 5, length($4)-4));
    banks[bankNum++] = name;
    start[name] = s;
    end[name]   = e;
    size[name]  = e-s;

    if (name == "user") {
      userStart = s;
      userUsed = 0;
    }
    
    if (name == "kmem" ) {
      kernelUsed = 0;
      kernelStart = s;
    }

    if (debug)
      printf("Found Bank %s: %d - %d, size %d\n", name, s, e, e-s);
  }
}

/RAM/ {
  if (substr($3, 1, 4) == "RAM=") {
    sectionName = substr($2, 6, length($2)-5);
    bankName = substr($3, 5, length($3)-4);
    if (debug)
      printf("Found Section %s, Bank %s\n", sectionName, bankName);
    bank[sectionName] = bankName;
  }
}

/udata/ {
  if (($2 == "udata") && ($4 == "data")) {
   if(Hex2Dez(substr($3, 3, length($3)-2)) == userStart) {
     used = Hex2Dez(substr($5, 3, length($5)-2));
     userUsed += used;
     userStart += used;
   }
   if(Hex2Dez(substr($3, 3, length($3)-2)) == kernelStart) {
     used = Hex2Dez(substr($5, 3, length($5)-2));
     kernelUsed += used;
     kernelStart += used;
   }   
   
   bankName = bank[$1];
   if (bankName != "") {
     used = 0+Hex2Dez(substr($5, 3, length($5)-2));
     if(debug)
        printf("Bank %s: %d of %d used [%d%%]\n", bankName, used, size[bankName], (used*100)/size[bankName]);
   }
  }
}

END {
	 printf("Memory Utilisation:\n");
     printf("   Data : %d of %d bytes [%d%%] used.\n", userUsed, size["user"], (userUsed*100)/size ["user"]);
     printf("   Code : %d of %d bytes [%d%%] used.\n", highestCodeAddr, 0x8000, (highestCodeAddr*100)/0x8000);
}

