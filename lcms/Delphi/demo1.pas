unit demo1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, StdCtrls, ExtDlgs, lcmsdll, ComCtrls;

type
  TForm1 = class(TForm)

    Image1: TImage;
    Image2: TImage;
    Panel1: TPanel;
    Splitter1: TSplitter;
    Button2: TButton;
    ComboBoxInput: TComboBox;
    ComboBoxOutput: TComboBox;
    Label1: TLabel;
    Label2: TLabel;
    WBCompensation: TCheckBox;
    NoTransform: TCheckBox;
    RadioGroup1: TRadioGroup;
    OpenPictureDialog1: TOpenPictureDialog;
    Button1: TButton;
    ProgressBar1: TProgressBar;
    ComboBoxIntent: TComboBox;
    Label3: TLabel;

    procedure Button2Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
    function ComputeFlags: DWORD;

  public
   constructor Create(Owner: TComponent); Override;
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

CONST
IS_INPUT   = $1;
IS_DISPLAY = $2;
IS_COLORSPACE = $4;
IS_OUTPUT     = $8;
IS_ABSTRACT   = $10;


FUNCTION InSignatures(Signature:icProfileClassSignature; dwFlags: DWORD): Boolean;
BEGIN

        if (((dwFlags AND IS_DISPLAY) <> 0) AND (Signature = icSigDisplayClass)) then InSignatures  := TRUE
        else
        if (((dwFlags AND IS_OUTPUT) <> 0) AND (Signature = icSigOutputClass)) then InSignatures := TRUE
        else
        if (((dwFlags AND IS_INPUT) <> 0) AND (Signature = icSigInputClass)) then InSignatures := TRUE
        else
        if (((dwFlags AND IS_COLORSPACE) <> 0) AND (Signature = icSigColorSpaceClass)) then InSignatures := TRUE
        else
        if (((dwFlags AND IS_ABSTRACT) <> 0) AND (Signature = icSigAbstractClass)) then InSignatures := TRUE
        else
           InSignatures := FALSE
END;

PROCEDURE FillCombo(var Combo: TComboBox; Signatures: DWORD);
var
   Files, Descriptions : TStringList;
   Found: Integer;
   SearchRec: TSearchRec;
   Path, Profile: String;
   Dir: ARRAY[0..1024] OF Char;
   hProfile : cmsHPROFILE;
begin
       Files        := TStringList.Create;
       Descriptions := TStringList.Create;
       GetSystemDirectory(Dir, 1023);
       Path := String(Dir) + '\SPOOL\DRIVERS\COLOR\';
       Found := FindFirst(Path + '*.icm', faAnyFile, SearchRec);
       while Found = 0 do
       begin
            Profile := Path + SearchRec.Name;
            hProfile := cmsOpenProfileFromFile(PChar(Profile), 'r');
            if (hProfile <> NIL) THEN
            begin

             if  ((cmsGetColorSpace(hProfile) = icSigRgbData) AND
                  InSignatures(cmsGetDeviceClass(hProfile), Signatures)) then
             begin
                          Descriptions.Add(cmsTakeProductDesc(hProfile));
                          Files.Add(Profile);
             end;
             cmsCloseProfile(hProfile);
            end;

            Found := FindNext(SearchRec);

       end;
       FindClose(SearchRec);
       Combo.Items := Descriptions;
       Combo.Tag   := Integer(Files);
end;


constructor TForm1.Create(Owner: TComponent);
begin
     inherited Create(Owner);
     FillCombo(ComboBoxInput, IS_INPUT OR IS_COLORSPACE OR IS_DISPLAY);
     FillCombo(ComboBoxOutput, $FFFF {IS_DISPLAY} );
     ComboBoxIntent.ItemIndex := INTENT_PERCEPTUAL;
end;



procedure TForm1.Button2Click(Sender: TObject);
begin


     if OpenPictureDialog1.Execute then begin
            Image1.Picture.LoadFromFile(OpenPictureDialog1.FileName);
            Image1.Picture.Bitmap.PixelFormat := pf24bit;
            Image2.Picture.Bitmap := TBitmap.Create;
            Image2.Picture.Bitmap.PixelFormat := pf24bit;
            Image2.Picture.Bitmap.Dormant;
            Image2.Picture.Bitmap.width  := Image1.Picture.Bitmap.width;
            Image2.Picture.Bitmap.height := Image1.Picture.Bitmap.height;
            end
end;

function SelectedFile(var Combo: TComboBox):string;
var List: TStringList;
    n: Integer;
begin

     List := TStringList(Combo.Tag);
     n := Combo.ItemIndex;
     if (n >= 0) then
        SelectedFile := List.Strings[n]
     else
         SelectedFile := ''
end;


function TForm1.ComputeFlags: DWORD;
var dwFlags: DWORD;
begin
     dwFlags := 0;
     if (WBCompensation.Checked) then
     begin
        dwFlags := dwFlags OR cmsFLAGS_WHITEBLACKCOMPENSATION
     end;

     if (NoTransform.Checked) then
     begin
        dwFlags := dwFlags OR cmsFLAGS_NULLTRANSFORM
     end;


     case RadioGroup1.ItemIndex of
     0: dwFlags := dwFlags OR cmsFLAGS_NOTPRECALC;
     2: dwFlags := dwFlags OR cmsFLAGS_HIGHRESPRECALC;
     3: dwFlags := dwFlags OR cmsFLAGS_LOWRESPRECALC;
     end;
     
     ComputeFlags := dwFlags
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  Source, Dest: String;
  hSrc, hDest : cmsHPROFILE;
  xform: cmsHTRANSFORM;
  i, PicW, PicH: Integer;
  Intent: Integer;
  dwFlags : DWORD;
begin


     Source := SelectedFile(ComboBoxInput);
     Dest   := SelectedFile(ComboBoxOutput);

     dwFlags := ComputeFlags;

     Intent := ComboBoxIntent.ItemIndex;

     if (Source <> '') AND (Dest <> '') then
     begin
          hSrc := cmsOpenProfileFromFile(PChar(Source), 'r');
          hDest:= cmsOpenProfileFromFile(PChar(Dest), 'r');

          xform := cmsCreateTransform(hSrc, TYPE_BGR_8, hDest, TYPE_BGR_8,
                                           Intent, dwFlags);


          PicW := Image2.Picture.Width;
          PicH := Image2.Picture.Height;
          ProgressBar1.Min := 0;
          ProgressBar1.Max := PicH;
          ProgressBar1.Step := 1;

          for i:= 0 TO (PicH -1) do
          begin
               if ((i MOD 100) = 0) then
                              ProgressBar1.Position := i;

               cmsDoTransform(xform,
                              Image1.Picture.Bitmap.Scanline[i],
                              Image2.Picture.Bitmap.Scanline[i],
                              PicW);

          end;
          ProgressBar1.Position := PicH;

          cmsDeleteTransform(xform);
          cmsCloseProfile(hSrc);
          cmsCloseProfile(hDest);
          Image2.Repaint;
          ProgressBar1.Position := 0;
     end
end;

end.
