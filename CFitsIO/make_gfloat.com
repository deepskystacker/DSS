$ ! Command file to build the CFITSIO library on a VMS systems (VAX or Alpha)
$ !  This uses the default /float=G_FLOAT option on the Alpha
$ set verify
$ cc buffers.c
$ cc cfileio.c
$ cc checksum.c
$ cc compress.c
$ cc drvrfile.c
$ cc drvrmem.c
$ ! cc drvrnet.c   not currently supported on VMS
$ ! cc drvsmem.c   not currently supported on VMS
$ cc editcol.c
$ cc edithdu.c
$ cc eval_f.c
$ cc eval_l.c
$ cc eval_y.c
$ cc fitscore.c
$ cc f77_wrap1.c
$ cc f77_wrap2.c
$ cc f77_wrap3.c
$ cc f77_wrap4.c
$ cc getcol.c
$ cc getcolb.c
$ cc getcolsb.c
$ cc getcoli.c
$ cc getcolj.c
$ cc getcolui.c
$ cc getcoluj.c
$ cc getcoluk.c
$ cc getcolk.c
$ cc getcole.c
$ cc getcold.c
$ cc getcoll.c
$ cc getcols.c
$ cc getkey.c
$ cc group.c
$ cc grparser.c
$ cc histo.c
$ cc iraffits.c
$ cc modkey.c
$ cc putcol.c
$ cc putcolb.c
$ cc putcolsb.c
$ cc putcoli.c
$ cc putcolj.c
$ cc putcolk.c
$ cc putcolui.c
$ cc putcoluj.c
$ cc putcoluk.c
$ cc putcole.c
$ cc putcold.c
$ cc putcols.c
$ cc putcoll.c
$ cc putcolu.c
$ cc putkey.c
$ cc region.c
$ cc scalnull.c
$ cc swapproc.c
$ cc wcsutil.c
$ cc wcssub.c
$ cc imcompress.c
$ cc quantize.c
$ cc ricecomp.c
$ cc pliocomp.c
$ cc/float=d_float fits_hcompress.c
$ cc/float=d_float fits_hdecompress.c
$ lib/create cfitsio buffers,cfileio,checksum,compress,drvrfile,drvrmem
$ lib/insert cfitsio editcol,edithdu,eval_f,eval_l,eval_y
$ lib/insert cfitsio f77_wrap1,f77_wrap2,f77_wrap3,f77_wrap4
$ lib/insert cfitsio fitscore,getcol,getcolb,getcoli,getcolj,getcolk,getcole
$ lib/insert cfitsio getcold,getcoll,getcols,getcolui,getcoluj,getcoluk,getcolsb
$ lib/insert cfitsio getkey,group,grparser,histo,iraffits,modkey,putcol,putcolb
$ lib/insert cfitsio putcoli,putcolj,putcolk,putcole,putcold,putcolui,putcolsb
$ lib/insert cfitsio putcoluj,putcoluk,putcols,putcoll,putcolu,putkey,region
$ lib/insert cfitsio scalnull,swapproc,wcsutil,wcssub,imcompress
$ lib/insert cfitsio quantize,ricecomp,pliocomp,fits_hcompress,fits_hdecompress
$ ! 
$ if (F$GETSYI("ARCH_NAME") .eqs. "VAX") then goto VAX
$   set noverify
$   exit
$ !
$ VAX:
$ ! add macro routines not needed on Alpha and only used on VAX
$   macro vmsieeer.mar
$   macro vmsieeed.mar
$   lib/insert cfitsio vmsieeer,vmsieeed
$   set noverify
$   exit
