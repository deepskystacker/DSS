$ ! Command file to build the CFITSIO library on a VMS systems (VAX or Alpha)
$ set verify
$ cc/float=ieee_float buffers.c
$ cc/float=ieee_float cfileio.c
$ cc/float=ieee_float checksum.c
$ cc/float=ieee_float compress.c
$ cc/float=ieee_float drvrfile.c
$ cc/float=ieee_float drvrmem.c
$ ! cc/float=ieee_float drvrnet.c   not currently supported on VMS
$ ! cc/float=ieee_float drvsmem.c   not currently supported on VMS
$ cc/float=ieee_float editcol.c
$ cc/float=ieee_float edithdu.c
$ cc/float=ieee_float eval_f.c
$ cc/float=ieee_float eval_l.c
$ cc/float=ieee_float eval_y.c
$ cc/float=ieee_float fitscore.c
$ cc/float=ieee_float f77_wrap1.c
$ cc/float=ieee_float f77_wrap2.c
$ cc/float=ieee_float f77_wrap3.c
$ cc/float=ieee_float f77_wrap4.c
$ cc/float=ieee_float getcol.c
$ cc/float=ieee_float getcolb.c
$ cc/float=ieee_float getcolsb.c
$ cc/float=ieee_float getcoli.c
$ cc/float=ieee_float getcolj.c
$ cc/float=ieee_float getcolui.c
$ cc/float=ieee_float getcoluj.c
$ cc/float=ieee_float getcoluk.c
$ cc/float=ieee_float getcolk.c
$ cc/float=ieee_float getcole.c
$ cc/float=ieee_float getcold.c
$ cc/float=ieee_float getcoll.c
$ cc/float=ieee_float getcols.c
$ cc/float=ieee_float getkey.c
$ cc/float=ieee_float group.c
$ cc/float=ieee_float grparser.c
$ cc/float=ieee_float histo.c
$ cc/float=ieee_float iraffits.c
$ cc/float=ieee_float modkey.c
$ cc/float=ieee_float putcol.c
$ cc/float=ieee_float putcolb.c
$ cc/float=ieee_float putcolsb.c
$ cc/float=ieee_float putcoli.c
$ cc/float=ieee_float putcolj.c
$ cc/float=ieee_float putcolk.c
$ cc/float=ieee_float putcolui.c
$ cc/float=ieee_float putcoluj.c
$ cc/float=ieee_float putcoluk.c
$ cc/float=ieee_float putcole.c
$ cc/float=ieee_float putcold.c
$ cc/float=ieee_float putcols.c
$ cc/float=ieee_float putcoll.c
$ cc/float=ieee_float putcolu.c
$ cc/float=ieee_float putkey.c
$ cc/float=ieee_float region.c
$ cc/float=ieee_float scalnull.c
$ cc/float=ieee_float swapproc.c
$ cc/float=ieee_float wcsutil.c
$ cc/float=ieee_float wcssub.c
$ cc/float=ieee_float imcompress.c
$ cc/float=ieee_float quantize.c
$ cc/float=ieee_float ricecomp.c
$ cc/float=ieee_float pliocomp.c
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
