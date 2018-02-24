$ ! Command file to build the CFITSIO library on a VMS systems (VAX or Alpha)
$ set verify
$ cc/float=d_float buffers.c
$ cc/float=d_float cfileio.c
$ cc/float=d_float checksum.c
$ cc/float=d_float compress.c
$ cc/float=d_float drvrfile.c
$ cc/float=d_float drvrmem.c
$ ! cc/float=d_float drvrnet.c   not currently supported on VMS
$ ! cc/float=d_float drvsmem.c   not currently supported on VMS
$ cc/float=d_float editcol.c
$ cc/float=d_float edithdu.c
$ cc/float=d_float eval_f.c
$ cc/float=d_float eval_l.c
$ cc/float=d_float eval_y.c
$ cc/float=d_float fitscore.c
$ cc/float=d_float f77_wrap1.c
$ cc/float=d_float f77_wrap2.c
$ cc/float=d_float f77_wrap3.c
$ cc/float=d_float f77_wrap4.c
$ cc/float=d_float getcol.c
$ cc/float=d_float getcolb.c
$ cc/float=d_float getcolsb.c
$ cc/float=d_float getcoli.c
$ cc/float=d_float getcolj.c
$ cc/float=d_float getcolui.c
$ cc/float=d_float getcoluj.c
$ cc/float=d_float getcoluk.c
$ cc/float=d_float getcolk.c
$ cc/float=d_float getcole.c
$ cc/float=d_float getcold.c
$ cc/float=d_float getcoll.c
$ cc/float=d_float getcols.c
$ cc/float=d_float getkey.c
$ cc/float=d_float group.c
$ cc/float=d_float grparser.c
$ cc/float=d_float histo.c
$ cc/float=d_float iraffits.c
$ cc/float=d_float modkey.c
$ cc/float=d_float putcol.c
$ cc/float=d_float putcolb.c
$ cc/float=d_float putcolsb.c
$ cc/float=d_float putcoli.c
$ cc/float=d_float putcolj.c
$ cc/float=d_float putcolk.c
$ cc/float=d_float putcolui.c
$ cc/float=d_float putcoluj.c
$ cc/float=d_float putcoluk.c
$ cc/float=d_float putcole.c
$ cc/float=d_float putcold.c
$ cc/float=d_float putcols.c
$ cc/float=d_float putcoll.c
$ cc/float=d_float putcolu.c
$ cc/float=d_float putkey.c
$ cc/float=d_float region.c
$ cc/float=d_float scalnull.c
$ cc/float=d_float swapproc.c
$ cc/float=d_float wcsutil.c
$ cc/float=d_float wcssub.c
$ cc/float=d_float imcompress.c
$ cc/float=d_float quantize.c
$ cc/float=d_float ricecomp.c
$ cc/float=d_float pliocomp.c
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
$ ! add C routine needed on Alpha to do D_FLOAT conversions
$   cc/float=d_float vmsieee.c
$   lib/insert cfitsio vmsieee
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
