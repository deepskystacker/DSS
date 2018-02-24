# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _lcms
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


class COLORW(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, COLORW, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, COLORW, name)
    __swig_setmethods__["w"] = _lcms.COLORW_w_set
    __swig_getmethods__["w"] = _lcms.COLORW_w_get
    if _newclass:w = property(_lcms.COLORW_w_get, _lcms.COLORW_w_set)
    def __init__(self, *args): 
        this = _lcms.new_COLORW(*args)
        try: self.this.append(this)
        except: self.this = this
    def __repr__(*args): return _lcms.COLORW___repr__(*args)
    def __getitem__(*args): return _lcms.COLORW___getitem__(*args)
    def __setitem__(*args): return _lcms.COLORW___setitem__(*args)
    __swig_destroy__ = _lcms.delete_COLORW
    __del__ = lambda self : None;
COLORW_swigregister = _lcms.COLORW_swigregister
COLORW_swigregister(COLORW)

class COLORB(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, COLORB, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, COLORB, name)
    __swig_setmethods__["b"] = _lcms.COLORB_b_set
    __swig_getmethods__["b"] = _lcms.COLORB_b_get
    if _newclass:b = property(_lcms.COLORB_b_get, _lcms.COLORB_b_set)
    def __init__(self, *args): 
        this = _lcms.new_COLORB(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_COLORB
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.COLORB___repr__(*args)
    def __getitem__(*args): return _lcms.COLORB___getitem__(*args)
    def __setitem__(*args): return _lcms.COLORB___setitem__(*args)
COLORB_swigregister = _lcms.COLORB_swigregister
COLORB_swigregister(COLORB)

class IT8(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, IT8, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, IT8, name)
    __swig_setmethods__["hIT8"] = _lcms.IT8_hIT8_set
    __swig_getmethods__["hIT8"] = _lcms.IT8_hIT8_get
    if _newclass:hIT8 = property(_lcms.IT8_hIT8_get, _lcms.IT8_hIT8_set)
    def __init__(self, *args): 
        this = _lcms.new_IT8(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_IT8
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.IT8___repr__(*args)
    def saveToFile(*args): return _lcms.IT8_saveToFile(*args)
    def tableCount(*args): return _lcms.IT8_tableCount(*args)
    def setTable(*args): return _lcms.IT8_setTable(*args)
    def getSheetType(*args): return _lcms.IT8_getSheetType(*args)
    def setSheetType(*args): return _lcms.IT8_setSheetType(*args)
    def addComment(*args): return _lcms.IT8_addComment(*args)
    def setProperty(*args): return _lcms.IT8_setProperty(*args)
    def setPropertyAsHex(*args): return _lcms.IT8_setPropertyAsHex(*args)
    def setPropertyUncooked(*args): return _lcms.IT8_setPropertyUncooked(*args)
    def getProperty(*args): return _lcms.IT8_getProperty(*args)
    def getPropertyAsDbl(*args): return _lcms.IT8_getPropertyAsDbl(*args)
    def getData(*args): return _lcms.IT8_getData(*args)
    def getDataAsDbl(*args): return _lcms.IT8_getDataAsDbl(*args)
    def setData(*args): return _lcms.IT8_setData(*args)
    def setDataFormat(*args): return _lcms.IT8_setDataFormat(*args)
    def getPatchName(*args): return _lcms.IT8_getPatchName(*args)
    def getDataFormat(*args): return _lcms.IT8_getDataFormat(*args)
    def enumDataFormat(*args): return _lcms.IT8_enumDataFormat(*args)
    def enumProperties(*args): return _lcms.IT8_enumProperties(*args)
    def setTableByLabel(*args): return _lcms.IT8_setTableByLabel(*args)
    def getRow(*args): return _lcms.IT8_getRow(*args)
    def getCol(*args): return _lcms.IT8_getCol(*args)
IT8_swigregister = _lcms.IT8_swigregister
IT8_swigregister(IT8)

NON_WINDOWS = _lcms.NON_WINDOWS
LCMS_VERSION = _lcms.LCMS_VERSION
class icCLutStruct(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, icCLutStruct, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, icCLutStruct, name)
    __repr__ = _swig_repr
    __swig_setmethods__["gridPoints"] = _lcms.icCLutStruct_gridPoints_set
    __swig_getmethods__["gridPoints"] = _lcms.icCLutStruct_gridPoints_get
    if _newclass:gridPoints = property(_lcms.icCLutStruct_gridPoints_get, _lcms.icCLutStruct_gridPoints_set)
    __swig_setmethods__["prec"] = _lcms.icCLutStruct_prec_set
    __swig_getmethods__["prec"] = _lcms.icCLutStruct_prec_get
    if _newclass:prec = property(_lcms.icCLutStruct_prec_get, _lcms.icCLutStruct_prec_set)
    __swig_setmethods__["pad1"] = _lcms.icCLutStruct_pad1_set
    __swig_getmethods__["pad1"] = _lcms.icCLutStruct_pad1_get
    if _newclass:pad1 = property(_lcms.icCLutStruct_pad1_get, _lcms.icCLutStruct_pad1_set)
    __swig_setmethods__["pad2"] = _lcms.icCLutStruct_pad2_set
    __swig_getmethods__["pad2"] = _lcms.icCLutStruct_pad2_get
    if _newclass:pad2 = property(_lcms.icCLutStruct_pad2_get, _lcms.icCLutStruct_pad2_set)
    __swig_setmethods__["pad3"] = _lcms.icCLutStruct_pad3_set
    __swig_getmethods__["pad3"] = _lcms.icCLutStruct_pad3_get
    if _newclass:pad3 = property(_lcms.icCLutStruct_pad3_get, _lcms.icCLutStruct_pad3_set)
    def __init__(self, *args): 
        this = _lcms.new_icCLutStruct(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_icCLutStruct
    __del__ = lambda self : None;
icCLutStruct_swigregister = _lcms.icCLutStruct_swigregister
icCLutStruct_swigregister(icCLutStruct)

class icLutAtoB(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, icLutAtoB, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, icLutAtoB, name)
    __repr__ = _swig_repr
    __swig_setmethods__["inputChan"] = _lcms.icLutAtoB_inputChan_set
    __swig_getmethods__["inputChan"] = _lcms.icLutAtoB_inputChan_get
    if _newclass:inputChan = property(_lcms.icLutAtoB_inputChan_get, _lcms.icLutAtoB_inputChan_set)
    __swig_setmethods__["outputChan"] = _lcms.icLutAtoB_outputChan_set
    __swig_getmethods__["outputChan"] = _lcms.icLutAtoB_outputChan_get
    if _newclass:outputChan = property(_lcms.icLutAtoB_outputChan_get, _lcms.icLutAtoB_outputChan_set)
    __swig_setmethods__["pad1"] = _lcms.icLutAtoB_pad1_set
    __swig_getmethods__["pad1"] = _lcms.icLutAtoB_pad1_get
    if _newclass:pad1 = property(_lcms.icLutAtoB_pad1_get, _lcms.icLutAtoB_pad1_set)
    __swig_setmethods__["pad2"] = _lcms.icLutAtoB_pad2_set
    __swig_getmethods__["pad2"] = _lcms.icLutAtoB_pad2_get
    if _newclass:pad2 = property(_lcms.icLutAtoB_pad2_get, _lcms.icLutAtoB_pad2_set)
    __swig_setmethods__["offsetB"] = _lcms.icLutAtoB_offsetB_set
    __swig_getmethods__["offsetB"] = _lcms.icLutAtoB_offsetB_get
    if _newclass:offsetB = property(_lcms.icLutAtoB_offsetB_get, _lcms.icLutAtoB_offsetB_set)
    __swig_setmethods__["offsetMat"] = _lcms.icLutAtoB_offsetMat_set
    __swig_getmethods__["offsetMat"] = _lcms.icLutAtoB_offsetMat_get
    if _newclass:offsetMat = property(_lcms.icLutAtoB_offsetMat_get, _lcms.icLutAtoB_offsetMat_set)
    __swig_setmethods__["offsetM"] = _lcms.icLutAtoB_offsetM_set
    __swig_getmethods__["offsetM"] = _lcms.icLutAtoB_offsetM_get
    if _newclass:offsetM = property(_lcms.icLutAtoB_offsetM_get, _lcms.icLutAtoB_offsetM_set)
    __swig_setmethods__["offsetC"] = _lcms.icLutAtoB_offsetC_set
    __swig_getmethods__["offsetC"] = _lcms.icLutAtoB_offsetC_get
    if _newclass:offsetC = property(_lcms.icLutAtoB_offsetC_get, _lcms.icLutAtoB_offsetC_set)
    __swig_setmethods__["offsetA"] = _lcms.icLutAtoB_offsetA_set
    __swig_getmethods__["offsetA"] = _lcms.icLutAtoB_offsetA_get
    if _newclass:offsetA = property(_lcms.icLutAtoB_offsetA_get, _lcms.icLutAtoB_offsetA_set)
    def __init__(self, *args): 
        this = _lcms.new_icLutAtoB(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_icLutAtoB
    __del__ = lambda self : None;
icLutAtoB_swigregister = _lcms.icLutAtoB_swigregister
icLutAtoB_swigregister(icLutAtoB)

class icLutBtoA(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, icLutBtoA, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, icLutBtoA, name)
    __repr__ = _swig_repr
    __swig_setmethods__["inputChan"] = _lcms.icLutBtoA_inputChan_set
    __swig_getmethods__["inputChan"] = _lcms.icLutBtoA_inputChan_get
    if _newclass:inputChan = property(_lcms.icLutBtoA_inputChan_get, _lcms.icLutBtoA_inputChan_set)
    __swig_setmethods__["outputChan"] = _lcms.icLutBtoA_outputChan_set
    __swig_getmethods__["outputChan"] = _lcms.icLutBtoA_outputChan_get
    if _newclass:outputChan = property(_lcms.icLutBtoA_outputChan_get, _lcms.icLutBtoA_outputChan_set)
    __swig_setmethods__["pad1"] = _lcms.icLutBtoA_pad1_set
    __swig_getmethods__["pad1"] = _lcms.icLutBtoA_pad1_get
    if _newclass:pad1 = property(_lcms.icLutBtoA_pad1_get, _lcms.icLutBtoA_pad1_set)
    __swig_setmethods__["pad2"] = _lcms.icLutBtoA_pad2_set
    __swig_getmethods__["pad2"] = _lcms.icLutBtoA_pad2_get
    if _newclass:pad2 = property(_lcms.icLutBtoA_pad2_get, _lcms.icLutBtoA_pad2_set)
    __swig_setmethods__["offsetB"] = _lcms.icLutBtoA_offsetB_set
    __swig_getmethods__["offsetB"] = _lcms.icLutBtoA_offsetB_get
    if _newclass:offsetB = property(_lcms.icLutBtoA_offsetB_get, _lcms.icLutBtoA_offsetB_set)
    __swig_setmethods__["offsetMat"] = _lcms.icLutBtoA_offsetMat_set
    __swig_getmethods__["offsetMat"] = _lcms.icLutBtoA_offsetMat_get
    if _newclass:offsetMat = property(_lcms.icLutBtoA_offsetMat_get, _lcms.icLutBtoA_offsetMat_set)
    __swig_setmethods__["offsetM"] = _lcms.icLutBtoA_offsetM_set
    __swig_getmethods__["offsetM"] = _lcms.icLutBtoA_offsetM_get
    if _newclass:offsetM = property(_lcms.icLutBtoA_offsetM_get, _lcms.icLutBtoA_offsetM_set)
    __swig_setmethods__["offsetC"] = _lcms.icLutBtoA_offsetC_set
    __swig_getmethods__["offsetC"] = _lcms.icLutBtoA_offsetC_get
    if _newclass:offsetC = property(_lcms.icLutBtoA_offsetC_get, _lcms.icLutBtoA_offsetC_set)
    __swig_setmethods__["offsetA"] = _lcms.icLutBtoA_offsetA_set
    __swig_getmethods__["offsetA"] = _lcms.icLutBtoA_offsetA_get
    if _newclass:offsetA = property(_lcms.icLutBtoA_offsetA_get, _lcms.icLutBtoA_offsetA_set)
    def __init__(self, *args): 
        this = _lcms.new_icLutBtoA(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_icLutBtoA
    __del__ = lambda self : None;
icLutBtoA_swigregister = _lcms.icLutBtoA_swigregister
icLutBtoA_swigregister(icLutBtoA)

MAXCHANNELS = _lcms.MAXCHANNELS
PT_ANY = _lcms.PT_ANY
PT_GRAY = _lcms.PT_GRAY
PT_RGB = _lcms.PT_RGB
PT_CMY = _lcms.PT_CMY
PT_CMYK = _lcms.PT_CMYK
PT_YCbCr = _lcms.PT_YCbCr
PT_YUV = _lcms.PT_YUV
PT_XYZ = _lcms.PT_XYZ
PT_Lab = _lcms.PT_Lab
PT_YUVK = _lcms.PT_YUVK
PT_HSV = _lcms.PT_HSV
PT_HLS = _lcms.PT_HLS
PT_Yxy = _lcms.PT_Yxy
PT_HiFi = _lcms.PT_HiFi
PT_HiFi7 = _lcms.PT_HiFi7
PT_HiFi8 = _lcms.PT_HiFi8
PT_HiFi9 = _lcms.PT_HiFi9
PT_HiFi10 = _lcms.PT_HiFi10
PT_HiFi11 = _lcms.PT_HiFi11
PT_HiFi12 = _lcms.PT_HiFi12
PT_HiFi13 = _lcms.PT_HiFi13
PT_HiFi14 = _lcms.PT_HiFi14
PT_HiFi15 = _lcms.PT_HiFi15
TYPE_GRAY_8 = _lcms.TYPE_GRAY_8
TYPE_GRAY_8_REV = _lcms.TYPE_GRAY_8_REV
TYPE_GRAY_16 = _lcms.TYPE_GRAY_16
TYPE_GRAY_16_REV = _lcms.TYPE_GRAY_16_REV
TYPE_GRAY_16_SE = _lcms.TYPE_GRAY_16_SE
TYPE_GRAYA_8 = _lcms.TYPE_GRAYA_8
TYPE_GRAYA_16 = _lcms.TYPE_GRAYA_16
TYPE_GRAYA_16_SE = _lcms.TYPE_GRAYA_16_SE
TYPE_GRAYA_8_PLANAR = _lcms.TYPE_GRAYA_8_PLANAR
TYPE_GRAYA_16_PLANAR = _lcms.TYPE_GRAYA_16_PLANAR
TYPE_RGB_8 = _lcms.TYPE_RGB_8
TYPE_RGB_8_PLANAR = _lcms.TYPE_RGB_8_PLANAR
TYPE_BGR_8 = _lcms.TYPE_BGR_8
TYPE_BGR_8_PLANAR = _lcms.TYPE_BGR_8_PLANAR
TYPE_RGB_16 = _lcms.TYPE_RGB_16
TYPE_RGB_16_PLANAR = _lcms.TYPE_RGB_16_PLANAR
TYPE_RGB_16_SE = _lcms.TYPE_RGB_16_SE
TYPE_BGR_16 = _lcms.TYPE_BGR_16
TYPE_BGR_16_PLANAR = _lcms.TYPE_BGR_16_PLANAR
TYPE_BGR_16_SE = _lcms.TYPE_BGR_16_SE
TYPE_RGBA_8 = _lcms.TYPE_RGBA_8
TYPE_RGBA_8_PLANAR = _lcms.TYPE_RGBA_8_PLANAR
TYPE_RGBA_16 = _lcms.TYPE_RGBA_16
TYPE_RGBA_16_PLANAR = _lcms.TYPE_RGBA_16_PLANAR
TYPE_RGBA_16_SE = _lcms.TYPE_RGBA_16_SE
TYPE_ARGB_8 = _lcms.TYPE_ARGB_8
TYPE_ARGB_16 = _lcms.TYPE_ARGB_16
TYPE_ABGR_8 = _lcms.TYPE_ABGR_8
TYPE_ABGR_16 = _lcms.TYPE_ABGR_16
TYPE_ABGR_16_PLANAR = _lcms.TYPE_ABGR_16_PLANAR
TYPE_ABGR_16_SE = _lcms.TYPE_ABGR_16_SE
TYPE_BGRA_8 = _lcms.TYPE_BGRA_8
TYPE_BGRA_16 = _lcms.TYPE_BGRA_16
TYPE_BGRA_16_SE = _lcms.TYPE_BGRA_16_SE
TYPE_CMY_8 = _lcms.TYPE_CMY_8
TYPE_CMY_8_PLANAR = _lcms.TYPE_CMY_8_PLANAR
TYPE_CMY_16 = _lcms.TYPE_CMY_16
TYPE_CMY_16_PLANAR = _lcms.TYPE_CMY_16_PLANAR
TYPE_CMY_16_SE = _lcms.TYPE_CMY_16_SE
TYPE_CMYK_8 = _lcms.TYPE_CMYK_8
TYPE_CMYKA_8 = _lcms.TYPE_CMYKA_8
TYPE_CMYK_8_REV = _lcms.TYPE_CMYK_8_REV
TYPE_YUVK_8 = _lcms.TYPE_YUVK_8
TYPE_CMYK_8_PLANAR = _lcms.TYPE_CMYK_8_PLANAR
TYPE_CMYK_16 = _lcms.TYPE_CMYK_16
TYPE_CMYK_16_REV = _lcms.TYPE_CMYK_16_REV
TYPE_YUVK_16 = _lcms.TYPE_YUVK_16
TYPE_CMYK_16_PLANAR = _lcms.TYPE_CMYK_16_PLANAR
TYPE_CMYK_16_SE = _lcms.TYPE_CMYK_16_SE
TYPE_KYMC_8 = _lcms.TYPE_KYMC_8
TYPE_KYMC_16 = _lcms.TYPE_KYMC_16
TYPE_KYMC_16_SE = _lcms.TYPE_KYMC_16_SE
TYPE_KCMY_8 = _lcms.TYPE_KCMY_8
TYPE_KCMY_8_REV = _lcms.TYPE_KCMY_8_REV
TYPE_KCMY_16 = _lcms.TYPE_KCMY_16
TYPE_KCMY_16_REV = _lcms.TYPE_KCMY_16_REV
TYPE_KCMY_16_SE = _lcms.TYPE_KCMY_16_SE
TYPE_CMYK5_8 = _lcms.TYPE_CMYK5_8
TYPE_CMYK5_16 = _lcms.TYPE_CMYK5_16
TYPE_CMYK5_16_SE = _lcms.TYPE_CMYK5_16_SE
TYPE_KYMC5_8 = _lcms.TYPE_KYMC5_8
TYPE_KYMC5_16 = _lcms.TYPE_KYMC5_16
TYPE_KYMC5_16_SE = _lcms.TYPE_KYMC5_16_SE
TYPE_CMYKcm_8 = _lcms.TYPE_CMYKcm_8
TYPE_CMYKcm_8_PLANAR = _lcms.TYPE_CMYKcm_8_PLANAR
TYPE_CMYKcm_16 = _lcms.TYPE_CMYKcm_16
TYPE_CMYKcm_16_PLANAR = _lcms.TYPE_CMYKcm_16_PLANAR
TYPE_CMYKcm_16_SE = _lcms.TYPE_CMYKcm_16_SE
TYPE_CMYK7_8 = _lcms.TYPE_CMYK7_8
TYPE_CMYK7_16 = _lcms.TYPE_CMYK7_16
TYPE_CMYK7_16_SE = _lcms.TYPE_CMYK7_16_SE
TYPE_KYMC7_8 = _lcms.TYPE_KYMC7_8
TYPE_KYMC7_16 = _lcms.TYPE_KYMC7_16
TYPE_KYMC7_16_SE = _lcms.TYPE_KYMC7_16_SE
TYPE_CMYK8_8 = _lcms.TYPE_CMYK8_8
TYPE_CMYK8_16 = _lcms.TYPE_CMYK8_16
TYPE_CMYK8_16_SE = _lcms.TYPE_CMYK8_16_SE
TYPE_KYMC8_8 = _lcms.TYPE_KYMC8_8
TYPE_KYMC8_16 = _lcms.TYPE_KYMC8_16
TYPE_KYMC8_16_SE = _lcms.TYPE_KYMC8_16_SE
TYPE_CMYK9_8 = _lcms.TYPE_CMYK9_8
TYPE_CMYK9_16 = _lcms.TYPE_CMYK9_16
TYPE_CMYK9_16_SE = _lcms.TYPE_CMYK9_16_SE
TYPE_KYMC9_8 = _lcms.TYPE_KYMC9_8
TYPE_KYMC9_16 = _lcms.TYPE_KYMC9_16
TYPE_KYMC9_16_SE = _lcms.TYPE_KYMC9_16_SE
TYPE_CMYK10_8 = _lcms.TYPE_CMYK10_8
TYPE_CMYK10_16 = _lcms.TYPE_CMYK10_16
TYPE_CMYK10_16_SE = _lcms.TYPE_CMYK10_16_SE
TYPE_KYMC10_8 = _lcms.TYPE_KYMC10_8
TYPE_KYMC10_16 = _lcms.TYPE_KYMC10_16
TYPE_KYMC10_16_SE = _lcms.TYPE_KYMC10_16_SE
TYPE_CMYK11_8 = _lcms.TYPE_CMYK11_8
TYPE_CMYK11_16 = _lcms.TYPE_CMYK11_16
TYPE_CMYK11_16_SE = _lcms.TYPE_CMYK11_16_SE
TYPE_KYMC11_8 = _lcms.TYPE_KYMC11_8
TYPE_KYMC11_16 = _lcms.TYPE_KYMC11_16
TYPE_KYMC11_16_SE = _lcms.TYPE_KYMC11_16_SE
TYPE_CMYK12_8 = _lcms.TYPE_CMYK12_8
TYPE_CMYK12_16 = _lcms.TYPE_CMYK12_16
TYPE_CMYK12_16_SE = _lcms.TYPE_CMYK12_16_SE
TYPE_KYMC12_8 = _lcms.TYPE_KYMC12_8
TYPE_KYMC12_16 = _lcms.TYPE_KYMC12_16
TYPE_KYMC12_16_SE = _lcms.TYPE_KYMC12_16_SE
TYPE_XYZ_16 = _lcms.TYPE_XYZ_16
TYPE_Lab_8 = _lcms.TYPE_Lab_8
TYPE_ALab_8 = _lcms.TYPE_ALab_8
TYPE_Lab_16 = _lcms.TYPE_Lab_16
TYPE_Yxy_16 = _lcms.TYPE_Yxy_16
TYPE_YCbCr_8 = _lcms.TYPE_YCbCr_8
TYPE_YCbCr_8_PLANAR = _lcms.TYPE_YCbCr_8_PLANAR
TYPE_YCbCr_16 = _lcms.TYPE_YCbCr_16
TYPE_YCbCr_16_PLANAR = _lcms.TYPE_YCbCr_16_PLANAR
TYPE_YCbCr_16_SE = _lcms.TYPE_YCbCr_16_SE
TYPE_YUV_8 = _lcms.TYPE_YUV_8
TYPE_YUV_8_PLANAR = _lcms.TYPE_YUV_8_PLANAR
TYPE_YUV_16 = _lcms.TYPE_YUV_16
TYPE_YUV_16_PLANAR = _lcms.TYPE_YUV_16_PLANAR
TYPE_YUV_16_SE = _lcms.TYPE_YUV_16_SE
TYPE_HLS_8 = _lcms.TYPE_HLS_8
TYPE_HLS_8_PLANAR = _lcms.TYPE_HLS_8_PLANAR
TYPE_HLS_16 = _lcms.TYPE_HLS_16
TYPE_HLS_16_PLANAR = _lcms.TYPE_HLS_16_PLANAR
TYPE_HLS_16_SE = _lcms.TYPE_HLS_16_SE
TYPE_HSV_8 = _lcms.TYPE_HSV_8
TYPE_HSV_8_PLANAR = _lcms.TYPE_HSV_8_PLANAR
TYPE_HSV_16 = _lcms.TYPE_HSV_16
TYPE_HSV_16_PLANAR = _lcms.TYPE_HSV_16_PLANAR
TYPE_HSV_16_SE = _lcms.TYPE_HSV_16_SE
TYPE_NAMED_COLOR_INDEX = _lcms.TYPE_NAMED_COLOR_INDEX
TYPE_XYZ_DBL = _lcms.TYPE_XYZ_DBL
TYPE_Lab_DBL = _lcms.TYPE_Lab_DBL
TYPE_GRAY_DBL = _lcms.TYPE_GRAY_DBL
TYPE_RGB_DBL = _lcms.TYPE_RGB_DBL
TYPE_CMYK_DBL = _lcms.TYPE_CMYK_DBL
class LCMSGAMMAPARAMS(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, LCMSGAMMAPARAMS, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, LCMSGAMMAPARAMS, name)
    __repr__ = _swig_repr
    __swig_setmethods__["Crc32"] = _lcms.LCMSGAMMAPARAMS_Crc32_set
    __swig_getmethods__["Crc32"] = _lcms.LCMSGAMMAPARAMS_Crc32_get
    if _newclass:Crc32 = property(_lcms.LCMSGAMMAPARAMS_Crc32_get, _lcms.LCMSGAMMAPARAMS_Crc32_set)
    __swig_setmethods__["Type"] = _lcms.LCMSGAMMAPARAMS_Type_set
    __swig_getmethods__["Type"] = _lcms.LCMSGAMMAPARAMS_Type_get
    if _newclass:Type = property(_lcms.LCMSGAMMAPARAMS_Type_get, _lcms.LCMSGAMMAPARAMS_Type_set)
    __swig_setmethods__["Params"] = _lcms.LCMSGAMMAPARAMS_Params_set
    __swig_getmethods__["Params"] = _lcms.LCMSGAMMAPARAMS_Params_get
    if _newclass:Params = property(_lcms.LCMSGAMMAPARAMS_Params_get, _lcms.LCMSGAMMAPARAMS_Params_set)
    def __init__(self, *args): 
        this = _lcms.new_LCMSGAMMAPARAMS(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_LCMSGAMMAPARAMS
    __del__ = lambda self : None;
LCMSGAMMAPARAMS_swigregister = _lcms.LCMSGAMMAPARAMS_swigregister
LCMSGAMMAPARAMS_swigregister(LCMSGAMMAPARAMS)

class GAMMATABLE(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, GAMMATABLE, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, GAMMATABLE, name)
    __swig_setmethods__["Seed"] = _lcms.GAMMATABLE_Seed_set
    __swig_getmethods__["Seed"] = _lcms.GAMMATABLE_Seed_get
    if _newclass:Seed = property(_lcms.GAMMATABLE_Seed_get, _lcms.GAMMATABLE_Seed_set)
    __swig_setmethods__["nEntries"] = _lcms.GAMMATABLE_nEntries_set
    __swig_getmethods__["nEntries"] = _lcms.GAMMATABLE_nEntries_get
    if _newclass:nEntries = property(_lcms.GAMMATABLE_nEntries_get, _lcms.GAMMATABLE_nEntries_set)
    __swig_setmethods__["GammaTable"] = _lcms.GAMMATABLE_GammaTable_set
    __swig_getmethods__["GammaTable"] = _lcms.GAMMATABLE_GammaTable_get
    if _newclass:GammaTable = property(_lcms.GAMMATABLE_GammaTable_get, _lcms.GAMMATABLE_GammaTable_set)
    def __init__(self, *args): 
        this = _lcms.new_GAMMATABLE(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_GAMMATABLE
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.GAMMATABLE___repr__(*args)
    def __getitem__(*args): return _lcms.GAMMATABLE___getitem__(*args)
    def __setitem__(*args): return _lcms.GAMMATABLE___setitem__(*args)
GAMMATABLE_swigregister = _lcms.GAMMATABLE_swigregister
GAMMATABLE_swigregister(GAMMATABLE)

class VEC3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, VEC3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, VEC3, name)
    __swig_setmethods__["n"] = _lcms.VEC3_n_set
    __swig_getmethods__["n"] = _lcms.VEC3_n_get
    if _newclass:n = property(_lcms.VEC3_n_get, _lcms.VEC3_n_set)
    def __init__(self, *args): 
        this = _lcms.new_VEC3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_VEC3
    __del__ = lambda self : None;
    def __getitem__(*args): return _lcms.VEC3___getitem__(*args)
    def __setitem__(*args): return _lcms.VEC3___setitem__(*args)
    def __repr__(*args): return _lcms.VEC3___repr__(*args)
VEC3_swigregister = _lcms.VEC3_swigregister
VEC3_swigregister(VEC3)

class MAT3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MAT3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MAT3, name)
    __repr__ = _swig_repr
    __swig_setmethods__["v"] = _lcms.MAT3_v_set
    __swig_getmethods__["v"] = _lcms.MAT3_v_get
    if _newclass:v = property(_lcms.MAT3_v_get, _lcms.MAT3_v_set)
    def __init__(self, *args): 
        this = _lcms.new_MAT3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_MAT3
    __del__ = lambda self : None;
MAT3_swigregister = _lcms.MAT3_swigregister
MAT3_swigregister(MAT3)

class cmsCIEXYZ(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsCIEXYZ, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsCIEXYZ, name)
    __swig_setmethods__["X"] = _lcms.cmsCIEXYZ_X_set
    __swig_getmethods__["X"] = _lcms.cmsCIEXYZ_X_get
    if _newclass:X = property(_lcms.cmsCIEXYZ_X_get, _lcms.cmsCIEXYZ_X_set)
    __swig_setmethods__["Y"] = _lcms.cmsCIEXYZ_Y_set
    __swig_getmethods__["Y"] = _lcms.cmsCIEXYZ_Y_get
    if _newclass:Y = property(_lcms.cmsCIEXYZ_Y_get, _lcms.cmsCIEXYZ_Y_set)
    __swig_setmethods__["Z"] = _lcms.cmsCIEXYZ_Z_set
    __swig_getmethods__["Z"] = _lcms.cmsCIEXYZ_Z_get
    if _newclass:Z = property(_lcms.cmsCIEXYZ_Z_get, _lcms.cmsCIEXYZ_Z_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsCIEXYZ(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsCIEXYZ
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.cmsCIEXYZ___repr__(*args)
cmsCIEXYZ_swigregister = _lcms.cmsCIEXYZ_swigregister
cmsCIEXYZ_swigregister(cmsCIEXYZ)

class cmsCIExyY(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsCIExyY, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsCIExyY, name)
    __swig_setmethods__["x"] = _lcms.cmsCIExyY_x_set
    __swig_getmethods__["x"] = _lcms.cmsCIExyY_x_get
    if _newclass:x = property(_lcms.cmsCIExyY_x_get, _lcms.cmsCIExyY_x_set)
    __swig_setmethods__["y"] = _lcms.cmsCIExyY_y_set
    __swig_getmethods__["y"] = _lcms.cmsCIExyY_y_get
    if _newclass:y = property(_lcms.cmsCIExyY_y_get, _lcms.cmsCIExyY_y_set)
    __swig_setmethods__["Y"] = _lcms.cmsCIExyY_Y_set
    __swig_getmethods__["Y"] = _lcms.cmsCIExyY_Y_get
    if _newclass:Y = property(_lcms.cmsCIExyY_Y_get, _lcms.cmsCIExyY_Y_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsCIExyY(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsCIExyY
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.cmsCIExyY___repr__(*args)
cmsCIExyY_swigregister = _lcms.cmsCIExyY_swigregister
cmsCIExyY_swigregister(cmsCIExyY)

class cmsCIELab(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsCIELab, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsCIELab, name)
    __swig_setmethods__["L"] = _lcms.cmsCIELab_L_set
    __swig_getmethods__["L"] = _lcms.cmsCIELab_L_get
    if _newclass:L = property(_lcms.cmsCIELab_L_get, _lcms.cmsCIELab_L_set)
    __swig_setmethods__["a"] = _lcms.cmsCIELab_a_set
    __swig_getmethods__["a"] = _lcms.cmsCIELab_a_get
    if _newclass:a = property(_lcms.cmsCIELab_a_get, _lcms.cmsCIELab_a_set)
    __swig_setmethods__["b"] = _lcms.cmsCIELab_b_set
    __swig_getmethods__["b"] = _lcms.cmsCIELab_b_get
    if _newclass:b = property(_lcms.cmsCIELab_b_get, _lcms.cmsCIELab_b_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsCIELab(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsCIELab
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.cmsCIELab___repr__(*args)
    def __cmp__(*args): return _lcms.cmsCIELab___cmp__(*args)
cmsCIELab_swigregister = _lcms.cmsCIELab_swigregister
cmsCIELab_swigregister(cmsCIELab)

class cmsCIELCh(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsCIELCh, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsCIELCh, name)
    __swig_setmethods__["L"] = _lcms.cmsCIELCh_L_set
    __swig_getmethods__["L"] = _lcms.cmsCIELCh_L_get
    if _newclass:L = property(_lcms.cmsCIELCh_L_get, _lcms.cmsCIELCh_L_set)
    __swig_setmethods__["C"] = _lcms.cmsCIELCh_C_set
    __swig_getmethods__["C"] = _lcms.cmsCIELCh_C_get
    if _newclass:C = property(_lcms.cmsCIELCh_C_get, _lcms.cmsCIELCh_C_set)
    __swig_setmethods__["h"] = _lcms.cmsCIELCh_h_set
    __swig_getmethods__["h"] = _lcms.cmsCIELCh_h_get
    if _newclass:h = property(_lcms.cmsCIELCh_h_get, _lcms.cmsCIELCh_h_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsCIELCh(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsCIELCh
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.cmsCIELCh___repr__(*args)
cmsCIELCh_swigregister = _lcms.cmsCIELCh_swigregister
cmsCIELCh_swigregister(cmsCIELCh)

class cmsJCh(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsJCh, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsJCh, name)
    __swig_setmethods__["J"] = _lcms.cmsJCh_J_set
    __swig_getmethods__["J"] = _lcms.cmsJCh_J_get
    if _newclass:J = property(_lcms.cmsJCh_J_get, _lcms.cmsJCh_J_set)
    __swig_setmethods__["C"] = _lcms.cmsJCh_C_set
    __swig_getmethods__["C"] = _lcms.cmsJCh_C_get
    if _newclass:C = property(_lcms.cmsJCh_C_get, _lcms.cmsJCh_C_set)
    __swig_setmethods__["h"] = _lcms.cmsJCh_h_set
    __swig_getmethods__["h"] = _lcms.cmsJCh_h_get
    if _newclass:h = property(_lcms.cmsJCh_h_get, _lcms.cmsJCh_h_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsJCh(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsJCh
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.cmsJCh___repr__(*args)
cmsJCh_swigregister = _lcms.cmsJCh_swigregister
cmsJCh_swigregister(cmsJCh)

class cmsCIEXYZTRIPLE(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsCIEXYZTRIPLE, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsCIEXYZTRIPLE, name)
    __swig_setmethods__["Red"] = _lcms.cmsCIEXYZTRIPLE_Red_set
    __swig_getmethods__["Red"] = _lcms.cmsCIEXYZTRIPLE_Red_get
    if _newclass:Red = property(_lcms.cmsCIEXYZTRIPLE_Red_get, _lcms.cmsCIEXYZTRIPLE_Red_set)
    __swig_setmethods__["Green"] = _lcms.cmsCIEXYZTRIPLE_Green_set
    __swig_getmethods__["Green"] = _lcms.cmsCIEXYZTRIPLE_Green_get
    if _newclass:Green = property(_lcms.cmsCIEXYZTRIPLE_Green_get, _lcms.cmsCIEXYZTRIPLE_Green_set)
    __swig_setmethods__["Blue"] = _lcms.cmsCIEXYZTRIPLE_Blue_set
    __swig_getmethods__["Blue"] = _lcms.cmsCIEXYZTRIPLE_Blue_get
    if _newclass:Blue = property(_lcms.cmsCIEXYZTRIPLE_Blue_get, _lcms.cmsCIEXYZTRIPLE_Blue_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsCIEXYZTRIPLE(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsCIEXYZTRIPLE
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.cmsCIEXYZTRIPLE___repr__(*args)
cmsCIEXYZTRIPLE_swigregister = _lcms.cmsCIEXYZTRIPLE_swigregister
cmsCIEXYZTRIPLE_swigregister(cmsCIEXYZTRIPLE)

class cmsCIExyYTRIPLE(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsCIExyYTRIPLE, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsCIExyYTRIPLE, name)
    __swig_setmethods__["Red"] = _lcms.cmsCIExyYTRIPLE_Red_set
    __swig_getmethods__["Red"] = _lcms.cmsCIExyYTRIPLE_Red_get
    if _newclass:Red = property(_lcms.cmsCIExyYTRIPLE_Red_get, _lcms.cmsCIExyYTRIPLE_Red_set)
    __swig_setmethods__["Green"] = _lcms.cmsCIExyYTRIPLE_Green_set
    __swig_getmethods__["Green"] = _lcms.cmsCIExyYTRIPLE_Green_get
    if _newclass:Green = property(_lcms.cmsCIExyYTRIPLE_Green_get, _lcms.cmsCIExyYTRIPLE_Green_set)
    __swig_setmethods__["Blue"] = _lcms.cmsCIExyYTRIPLE_Blue_set
    __swig_getmethods__["Blue"] = _lcms.cmsCIExyYTRIPLE_Blue_get
    if _newclass:Blue = property(_lcms.cmsCIExyYTRIPLE_Blue_get, _lcms.cmsCIExyYTRIPLE_Blue_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsCIExyYTRIPLE(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsCIExyYTRIPLE
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.cmsCIExyYTRIPLE___repr__(*args)
cmsCIExyYTRIPLE_swigregister = _lcms.cmsCIExyYTRIPLE_swigregister
cmsCIExyYTRIPLE_swigregister(cmsCIExyYTRIPLE)

D50X = _lcms.D50X
D50Y = _lcms.D50Y
D50Z = _lcms.D50Z
PERCEPTUAL_BLACK_X = _lcms.PERCEPTUAL_BLACK_X
PERCEPTUAL_BLACK_Y = _lcms.PERCEPTUAL_BLACK_Y
PERCEPTUAL_BLACK_Z = _lcms.PERCEPTUAL_BLACK_Z
cmsD50_XYZ = _lcms.cmsD50_XYZ
cmsD50_xyY = _lcms.cmsD50_xyY
cmsOpenProfileFromFile = _lcms.cmsOpenProfileFromFile
cmsOpenProfileFromMem = _lcms.cmsOpenProfileFromMem
cmsCloseProfile = _lcms.cmsCloseProfile
cmsCreateRGBProfile = _lcms.cmsCreateRGBProfile
cmsCreateGrayProfile = _lcms.cmsCreateGrayProfile
cmsCreateLinearizationDeviceLink = _lcms.cmsCreateLinearizationDeviceLink
cmsCreateInkLimitingDeviceLink = _lcms.cmsCreateInkLimitingDeviceLink
cmsCreateLabProfile = _lcms.cmsCreateLabProfile
cmsCreateLab4Profile = _lcms.cmsCreateLab4Profile
cmsCreateXYZProfile = _lcms.cmsCreateXYZProfile
cmsCreate_sRGBProfile = _lcms.cmsCreate_sRGBProfile
cmsCreateBCHSWabstractProfile = _lcms.cmsCreateBCHSWabstractProfile
cmsCreateNULLProfile = _lcms.cmsCreateNULLProfile
cmsXYZ2xyY = _lcms.cmsXYZ2xyY
cmsxyY2XYZ = _lcms.cmsxyY2XYZ
cmsXYZ2Lab = _lcms.cmsXYZ2Lab
cmsLab2XYZ = _lcms.cmsLab2XYZ
cmsLab2LCh = _lcms.cmsLab2LCh
cmsLCh2Lab = _lcms.cmsLCh2Lab
cmsDeltaE = _lcms.cmsDeltaE
cmsCIE94DeltaE = _lcms.cmsCIE94DeltaE
cmsBFDdeltaE = _lcms.cmsBFDdeltaE
cmsCMCdeltaE = _lcms.cmsCMCdeltaE
cmsCIE2000DeltaE = _lcms.cmsCIE2000DeltaE
cmsClampLab = _lcms.cmsClampLab
cmsWhitePointFromTemp = _lcms.cmsWhitePointFromTemp
cmsAdaptToIlluminant = _lcms.cmsAdaptToIlluminant
cmsBuildRGB2XYZtransferMatrix = _lcms.cmsBuildRGB2XYZtransferMatrix
AVG_SURROUND_4 = _lcms.AVG_SURROUND_4
AVG_SURROUND = _lcms.AVG_SURROUND
DIM_SURROUND = _lcms.DIM_SURROUND
DARK_SURROUND = _lcms.DARK_SURROUND
CUTSHEET_SURROUND = _lcms.CUTSHEET_SURROUND
D_CALCULATE = _lcms.D_CALCULATE
D_CALCULATE_DISCOUNT = _lcms.D_CALCULATE_DISCOUNT
class cmsViewingConditions(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsViewingConditions, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsViewingConditions, name)
    __swig_setmethods__["whitePoint"] = _lcms.cmsViewingConditions_whitePoint_set
    __swig_getmethods__["whitePoint"] = _lcms.cmsViewingConditions_whitePoint_get
    if _newclass:whitePoint = property(_lcms.cmsViewingConditions_whitePoint_get, _lcms.cmsViewingConditions_whitePoint_set)
    __swig_setmethods__["Yb"] = _lcms.cmsViewingConditions_Yb_set
    __swig_getmethods__["Yb"] = _lcms.cmsViewingConditions_Yb_get
    if _newclass:Yb = property(_lcms.cmsViewingConditions_Yb_get, _lcms.cmsViewingConditions_Yb_set)
    __swig_setmethods__["La"] = _lcms.cmsViewingConditions_La_set
    __swig_getmethods__["La"] = _lcms.cmsViewingConditions_La_get
    if _newclass:La = property(_lcms.cmsViewingConditions_La_get, _lcms.cmsViewingConditions_La_set)
    __swig_setmethods__["surround"] = _lcms.cmsViewingConditions_surround_set
    __swig_getmethods__["surround"] = _lcms.cmsViewingConditions_surround_get
    if _newclass:surround = property(_lcms.cmsViewingConditions_surround_get, _lcms.cmsViewingConditions_surround_set)
    __swig_setmethods__["D_value"] = _lcms.cmsViewingConditions_D_value_set
    __swig_getmethods__["D_value"] = _lcms.cmsViewingConditions_D_value_get
    if _newclass:D_value = property(_lcms.cmsViewingConditions_D_value_get, _lcms.cmsViewingConditions_D_value_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsViewingConditions(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsViewingConditions
    __del__ = lambda self : None;
    def __repr__(*args): return _lcms.cmsViewingConditions___repr__(*args)
cmsViewingConditions_swigregister = _lcms.cmsViewingConditions_swigregister
cmsViewingConditions_swigregister(cmsViewingConditions)

cmsCIECAM97sInit = _lcms.cmsCIECAM97sInit
cmsCIECAM97sDone = _lcms.cmsCIECAM97sDone
cmsCIECAM97sForward = _lcms.cmsCIECAM97sForward
cmsCIECAM97sReverse = _lcms.cmsCIECAM97sReverse
cmsCIECAM02Init = _lcms.cmsCIECAM02Init
cmsCIECAM02Done = _lcms.cmsCIECAM02Done
cmsCIECAM02Forward = _lcms.cmsCIECAM02Forward
cmsCIECAM02Reverse = _lcms.cmsCIECAM02Reverse
cmsBuildGamma = _lcms.cmsBuildGamma
cmsBuildParametricGamma = _lcms.cmsBuildParametricGamma
cmsAllocGamma = _lcms.cmsAllocGamma
cmsFreeGamma = _lcms.cmsFreeGamma
cmsFreeGammaTriple = _lcms.cmsFreeGammaTriple
cmsDupGamma = _lcms.cmsDupGamma
cmsReverseGamma = _lcms.cmsReverseGamma
cmsJoinGamma = _lcms.cmsJoinGamma
cmsJoinGammaEx = _lcms.cmsJoinGammaEx
cmsSmoothGamma = _lcms.cmsSmoothGamma
cmsEstimateGamma = _lcms.cmsEstimateGamma
cmsEstimateGammaEx = _lcms.cmsEstimateGammaEx
cmsReadICCGamma = _lcms.cmsReadICCGamma
cmsReadICCGammaReversed = _lcms.cmsReadICCGammaReversed
cmsTakeMediaWhitePoint = _lcms.cmsTakeMediaWhitePoint
cmsTakeMediaBlackPoint = _lcms.cmsTakeMediaBlackPoint
cmsTakeIluminant = _lcms.cmsTakeIluminant
cmsTakeColorants = _lcms.cmsTakeColorants
cmsTakeHeaderFlags = _lcms.cmsTakeHeaderFlags
cmsTakeHeaderAttributes = _lcms.cmsTakeHeaderAttributes
cmsSetLanguage = _lcms.cmsSetLanguage
cmsTakeProductName = _lcms.cmsTakeProductName
cmsTakeProductDesc = _lcms.cmsTakeProductDesc
cmsTakeProductInfo = _lcms.cmsTakeProductInfo
cmsTakeManufacturer = _lcms.cmsTakeManufacturer
cmsTakeModel = _lcms.cmsTakeModel
cmsTakeCopyright = _lcms.cmsTakeCopyright
cmsTakeProfileID = _lcms.cmsTakeProfileID
cmsTakeCreationDateTime = _lcms.cmsTakeCreationDateTime
cmsTakeCalibrationDateTime = _lcms.cmsTakeCalibrationDateTime
cmsIsTag = _lcms.cmsIsTag
cmsTakeRenderingIntent = _lcms.cmsTakeRenderingIntent
cmsTakeCharTargetData = _lcms.cmsTakeCharTargetData
cmsReadICCTextEx = _lcms.cmsReadICCTextEx
cmsReadICCText = _lcms.cmsReadICCText
LCMS_DESC_MAX = _lcms.LCMS_DESC_MAX
class cmsPSEQDESC(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsPSEQDESC, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsPSEQDESC, name)
    __repr__ = _swig_repr
    __swig_setmethods__["deviceMfg"] = _lcms.cmsPSEQDESC_deviceMfg_set
    __swig_getmethods__["deviceMfg"] = _lcms.cmsPSEQDESC_deviceMfg_get
    if _newclass:deviceMfg = property(_lcms.cmsPSEQDESC_deviceMfg_get, _lcms.cmsPSEQDESC_deviceMfg_set)
    __swig_setmethods__["deviceModel"] = _lcms.cmsPSEQDESC_deviceModel_set
    __swig_getmethods__["deviceModel"] = _lcms.cmsPSEQDESC_deviceModel_get
    if _newclass:deviceModel = property(_lcms.cmsPSEQDESC_deviceModel_get, _lcms.cmsPSEQDESC_deviceModel_set)
    __swig_setmethods__["attributes"] = _lcms.cmsPSEQDESC_attributes_set
    __swig_getmethods__["attributes"] = _lcms.cmsPSEQDESC_attributes_get
    if _newclass:attributes = property(_lcms.cmsPSEQDESC_attributes_get, _lcms.cmsPSEQDESC_attributes_set)
    __swig_setmethods__["technology"] = _lcms.cmsPSEQDESC_technology_set
    __swig_getmethods__["technology"] = _lcms.cmsPSEQDESC_technology_get
    if _newclass:technology = property(_lcms.cmsPSEQDESC_technology_get, _lcms.cmsPSEQDESC_technology_set)
    __swig_setmethods__["Manufacturer"] = _lcms.cmsPSEQDESC_Manufacturer_set
    __swig_getmethods__["Manufacturer"] = _lcms.cmsPSEQDESC_Manufacturer_get
    if _newclass:Manufacturer = property(_lcms.cmsPSEQDESC_Manufacturer_get, _lcms.cmsPSEQDESC_Manufacturer_set)
    __swig_setmethods__["Model"] = _lcms.cmsPSEQDESC_Model_set
    __swig_getmethods__["Model"] = _lcms.cmsPSEQDESC_Model_get
    if _newclass:Model = property(_lcms.cmsPSEQDESC_Model_get, _lcms.cmsPSEQDESC_Model_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsPSEQDESC(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsPSEQDESC
    __del__ = lambda self : None;
cmsPSEQDESC_swigregister = _lcms.cmsPSEQDESC_swigregister
cmsPSEQDESC_swigregister(cmsPSEQDESC)

class cmsSEQ(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsSEQ, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsSEQ, name)
    __repr__ = _swig_repr
    __swig_setmethods__["n"] = _lcms.cmsSEQ_n_set
    __swig_getmethods__["n"] = _lcms.cmsSEQ_n_get
    if _newclass:n = property(_lcms.cmsSEQ_n_get, _lcms.cmsSEQ_n_set)
    __swig_setmethods__["seq"] = _lcms.cmsSEQ_seq_set
    __swig_getmethods__["seq"] = _lcms.cmsSEQ_seq_get
    if _newclass:seq = property(_lcms.cmsSEQ_seq_get, _lcms.cmsSEQ_seq_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsSEQ(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsSEQ
    __del__ = lambda self : None;
cmsSEQ_swigregister = _lcms.cmsSEQ_swigregister
cmsSEQ_swigregister(cmsSEQ)

cmsReadProfileSequenceDescription = _lcms.cmsReadProfileSequenceDescription
cmsFreeProfileSequenceDescription = _lcms.cmsFreeProfileSequenceDescription
LCMSGAMUTMETHOD_SEGMENTMAXIMA = _lcms.LCMSGAMUTMETHOD_SEGMENTMAXIMA
LCMSGAMUTMETHOD_CONVEXHULL = _lcms.LCMSGAMUTMETHOD_CONVEXHULL
LCMSGAMUTMETHOD_ALPHASHAPE = _lcms.LCMSGAMUTMETHOD_ALPHASHAPE
LCMSGAMUT_PHYSICAL = _lcms.LCMSGAMUT_PHYSICAL
LCMSGAMUT_HP1 = _lcms.LCMSGAMUT_HP1
LCMSGAMUT_HP2 = _lcms.LCMSGAMUT_HP2
class cmsGAMUTEX(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsGAMUTEX, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsGAMUTEX, name)
    __repr__ = _swig_repr
    __swig_setmethods__["CoordSig"] = _lcms.cmsGAMUTEX_CoordSig_set
    __swig_getmethods__["CoordSig"] = _lcms.cmsGAMUTEX_CoordSig_get
    if _newclass:CoordSig = property(_lcms.cmsGAMUTEX_CoordSig_get, _lcms.cmsGAMUTEX_CoordSig_set)
    __swig_setmethods__["Method"] = _lcms.cmsGAMUTEX_Method_set
    __swig_getmethods__["Method"] = _lcms.cmsGAMUTEX_Method_get
    if _newclass:Method = property(_lcms.cmsGAMUTEX_Method_get, _lcms.cmsGAMUTEX_Method_set)
    __swig_setmethods__["Usage"] = _lcms.cmsGAMUTEX_Usage_set
    __swig_getmethods__["Usage"] = _lcms.cmsGAMUTEX_Usage_get
    if _newclass:Usage = property(_lcms.cmsGAMUTEX_Usage_get, _lcms.cmsGAMUTEX_Usage_set)
    __swig_setmethods__["Description"] = _lcms.cmsGAMUTEX_Description_set
    __swig_getmethods__["Description"] = _lcms.cmsGAMUTEX_Description_get
    if _newclass:Description = property(_lcms.cmsGAMUTEX_Description_get, _lcms.cmsGAMUTEX_Description_set)
    __swig_setmethods__["Vc"] = _lcms.cmsGAMUTEX_Vc_set
    __swig_getmethods__["Vc"] = _lcms.cmsGAMUTEX_Vc_get
    if _newclass:Vc = property(_lcms.cmsGAMUTEX_Vc_get, _lcms.cmsGAMUTEX_Vc_set)
    __swig_setmethods__["Count"] = _lcms.cmsGAMUTEX_Count_set
    __swig_getmethods__["Count"] = _lcms.cmsGAMUTEX_Count_get
    if _newclass:Count = property(_lcms.cmsGAMUTEX_Count_get, _lcms.cmsGAMUTEX_Count_set)
    __swig_setmethods__["Data"] = _lcms.cmsGAMUTEX_Data_set
    __swig_getmethods__["Data"] = _lcms.cmsGAMUTEX_Data_get
    if _newclass:Data = property(_lcms.cmsGAMUTEX_Data_get, _lcms.cmsGAMUTEX_Data_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsGAMUTEX(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsGAMUTEX
    __del__ = lambda self : None;
cmsGAMUTEX_swigregister = _lcms.cmsGAMUTEX_swigregister
cmsGAMUTEX_swigregister(cmsGAMUTEX)

cmsReadExtendedGamut = _lcms.cmsReadExtendedGamut
cmsFreeExtendedGamut = _lcms.cmsFreeExtendedGamut
_cmsICCcolorSpace = _lcms._cmsICCcolorSpace
_cmsLCMScolorSpace = _lcms._cmsLCMScolorSpace
_cmsChannelsOf = _lcms._cmsChannelsOf
_cmsIsMatrixShaper = _lcms._cmsIsMatrixShaper
LCMS_USED_AS_INPUT = _lcms.LCMS_USED_AS_INPUT
LCMS_USED_AS_OUTPUT = _lcms.LCMS_USED_AS_OUTPUT
LCMS_USED_AS_PROOF = _lcms.LCMS_USED_AS_PROOF
cmsIsIntentSupported = _lcms.cmsIsIntentSupported
cmsGetPCS = _lcms.cmsGetPCS
cmsGetColorSpace = _lcms.cmsGetColorSpace
cmsGetDeviceClass = _lcms.cmsGetDeviceClass
cmsGetProfileICCversion = _lcms.cmsGetProfileICCversion
cmsSetProfileICCversion = _lcms.cmsSetProfileICCversion
cmsGetTagCount = _lcms.cmsGetTagCount
cmsGetTagSignature = _lcms.cmsGetTagSignature
cmsSetDeviceClass = _lcms.cmsSetDeviceClass
cmsSetColorSpace = _lcms.cmsSetColorSpace
cmsSetPCS = _lcms.cmsSetPCS
cmsSetRenderingIntent = _lcms.cmsSetRenderingIntent
cmsSetHeaderFlags = _lcms.cmsSetHeaderFlags
cmsSetHeaderAttributes = _lcms.cmsSetHeaderAttributes
cmsSetProfileID = _lcms.cmsSetProfileID
INTENT_PERCEPTUAL = _lcms.INTENT_PERCEPTUAL
INTENT_RELATIVE_COLORIMETRIC = _lcms.INTENT_RELATIVE_COLORIMETRIC
INTENT_SATURATION = _lcms.INTENT_SATURATION
INTENT_ABSOLUTE_COLORIMETRIC = _lcms.INTENT_ABSOLUTE_COLORIMETRIC
cmsFLAGS_MATRIXINPUT = _lcms.cmsFLAGS_MATRIXINPUT
cmsFLAGS_MATRIXOUTPUT = _lcms.cmsFLAGS_MATRIXOUTPUT
cmsFLAGS_MATRIXONLY = _lcms.cmsFLAGS_MATRIXONLY
cmsFLAGS_NOWHITEONWHITEFIXUP = _lcms.cmsFLAGS_NOWHITEONWHITEFIXUP
cmsFLAGS_NOPRELINEARIZATION = _lcms.cmsFLAGS_NOPRELINEARIZATION
cmsFLAGS_GUESSDEVICECLASS = _lcms.cmsFLAGS_GUESSDEVICECLASS
cmsFLAGS_NOTCACHE = _lcms.cmsFLAGS_NOTCACHE
cmsFLAGS_NOTPRECALC = _lcms.cmsFLAGS_NOTPRECALC
cmsFLAGS_NULLTRANSFORM = _lcms.cmsFLAGS_NULLTRANSFORM
cmsFLAGS_HIGHRESPRECALC = _lcms.cmsFLAGS_HIGHRESPRECALC
cmsFLAGS_LOWRESPRECALC = _lcms.cmsFLAGS_LOWRESPRECALC
cmsFLAGS_WHITEBLACKCOMPENSATION = _lcms.cmsFLAGS_WHITEBLACKCOMPENSATION
cmsFLAGS_BLACKPOINTCOMPENSATION = _lcms.cmsFLAGS_BLACKPOINTCOMPENSATION
cmsFLAGS_GAMUTCHECK = _lcms.cmsFLAGS_GAMUTCHECK
cmsFLAGS_SOFTPROOFING = _lcms.cmsFLAGS_SOFTPROOFING
cmsFLAGS_PRESERVEBLACK = _lcms.cmsFLAGS_PRESERVEBLACK
cmsFLAGS_NODEFAULTRESOURCEDEF = _lcms.cmsFLAGS_NODEFAULTRESOURCEDEF
cmsCreateTransform = _lcms.cmsCreateTransform
cmsCreateProofingTransform = _lcms.cmsCreateProofingTransform
cmsCreateMultiprofileTransform = _lcms.cmsCreateMultiprofileTransform
cmsDeleteTransform = _lcms.cmsDeleteTransform
cmsDoTransform = _lcms.cmsDoTransform
cmsChangeBuffersFormat = _lcms.cmsChangeBuffersFormat
cmsSetAlarmCodes = _lcms.cmsSetAlarmCodes
cmsGetAlarmCodes = _lcms.cmsGetAlarmCodes
cmsSetAdaptationState = _lcms.cmsSetAdaptationState
LCMS_PRESERVE_PURE_K = _lcms.LCMS_PRESERVE_PURE_K
LCMS_PRESERVE_K_PLANE = _lcms.LCMS_PRESERVE_K_PLANE
cmsSetCMYKPreservationStrategy = _lcms.cmsSetCMYKPreservationStrategy
class cmsNAMEDCOLOR(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsNAMEDCOLOR, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsNAMEDCOLOR, name)
    __repr__ = _swig_repr
    __swig_setmethods__["Name"] = _lcms.cmsNAMEDCOLOR_Name_set
    __swig_getmethods__["Name"] = _lcms.cmsNAMEDCOLOR_Name_get
    if _newclass:Name = property(_lcms.cmsNAMEDCOLOR_Name_get, _lcms.cmsNAMEDCOLOR_Name_set)
    __swig_setmethods__["PCS"] = _lcms.cmsNAMEDCOLOR_PCS_set
    __swig_getmethods__["PCS"] = _lcms.cmsNAMEDCOLOR_PCS_get
    if _newclass:PCS = property(_lcms.cmsNAMEDCOLOR_PCS_get, _lcms.cmsNAMEDCOLOR_PCS_set)
    __swig_setmethods__["DeviceColorant"] = _lcms.cmsNAMEDCOLOR_DeviceColorant_set
    __swig_getmethods__["DeviceColorant"] = _lcms.cmsNAMEDCOLOR_DeviceColorant_get
    if _newclass:DeviceColorant = property(_lcms.cmsNAMEDCOLOR_DeviceColorant_get, _lcms.cmsNAMEDCOLOR_DeviceColorant_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsNAMEDCOLOR(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsNAMEDCOLOR
    __del__ = lambda self : None;
cmsNAMEDCOLOR_swigregister = _lcms.cmsNAMEDCOLOR_swigregister
cmsNAMEDCOLOR_swigregister(cmsNAMEDCOLOR)

class cmsNAMEDCOLORLIST(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, cmsNAMEDCOLORLIST, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, cmsNAMEDCOLORLIST, name)
    __repr__ = _swig_repr
    __swig_setmethods__["nColors"] = _lcms.cmsNAMEDCOLORLIST_nColors_set
    __swig_getmethods__["nColors"] = _lcms.cmsNAMEDCOLORLIST_nColors_get
    if _newclass:nColors = property(_lcms.cmsNAMEDCOLORLIST_nColors_get, _lcms.cmsNAMEDCOLORLIST_nColors_set)
    __swig_setmethods__["Allocated"] = _lcms.cmsNAMEDCOLORLIST_Allocated_set
    __swig_getmethods__["Allocated"] = _lcms.cmsNAMEDCOLORLIST_Allocated_get
    if _newclass:Allocated = property(_lcms.cmsNAMEDCOLORLIST_Allocated_get, _lcms.cmsNAMEDCOLORLIST_Allocated_set)
    __swig_setmethods__["ColorantCount"] = _lcms.cmsNAMEDCOLORLIST_ColorantCount_set
    __swig_getmethods__["ColorantCount"] = _lcms.cmsNAMEDCOLORLIST_ColorantCount_get
    if _newclass:ColorantCount = property(_lcms.cmsNAMEDCOLORLIST_ColorantCount_get, _lcms.cmsNAMEDCOLORLIST_ColorantCount_set)
    __swig_setmethods__["Prefix"] = _lcms.cmsNAMEDCOLORLIST_Prefix_set
    __swig_getmethods__["Prefix"] = _lcms.cmsNAMEDCOLORLIST_Prefix_get
    if _newclass:Prefix = property(_lcms.cmsNAMEDCOLORLIST_Prefix_get, _lcms.cmsNAMEDCOLORLIST_Prefix_set)
    __swig_setmethods__["Suffix"] = _lcms.cmsNAMEDCOLORLIST_Suffix_set
    __swig_getmethods__["Suffix"] = _lcms.cmsNAMEDCOLORLIST_Suffix_get
    if _newclass:Suffix = property(_lcms.cmsNAMEDCOLORLIST_Suffix_get, _lcms.cmsNAMEDCOLORLIST_Suffix_set)
    __swig_setmethods__["List"] = _lcms.cmsNAMEDCOLORLIST_List_set
    __swig_getmethods__["List"] = _lcms.cmsNAMEDCOLORLIST_List_get
    if _newclass:List = property(_lcms.cmsNAMEDCOLORLIST_List_get, _lcms.cmsNAMEDCOLORLIST_List_set)
    def __init__(self, *args): 
        this = _lcms.new_cmsNAMEDCOLORLIST(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_cmsNAMEDCOLORLIST
    __del__ = lambda self : None;
cmsNAMEDCOLORLIST_swigregister = _lcms.cmsNAMEDCOLORLIST_swigregister
cmsNAMEDCOLORLIST_swigregister(cmsNAMEDCOLORLIST)

cmsNamedColorCount = _lcms.cmsNamedColorCount
cmsNamedColorInfo = _lcms.cmsNamedColorInfo
cmsNamedColorIndex = _lcms.cmsNamedColorIndex
cmsReadColorantTable = _lcms.cmsReadColorantTable
cmsAddTag = _lcms.cmsAddTag
cmsTransform2DeviceLink = _lcms.cmsTransform2DeviceLink
_cmsSetLUTdepth = _lcms._cmsSetLUTdepth
cmsSaveProfile = _lcms.cmsSaveProfile
_cmsSaveProfileToMem = _lcms._cmsSaveProfileToMem
cmsGetPostScriptCSA = _lcms.cmsGetPostScriptCSA
cmsGetPostScriptCRD = _lcms.cmsGetPostScriptCRD
cmsGetPostScriptCRDEx = _lcms.cmsGetPostScriptCRDEx
LCMS_ERROR_ABORT = _lcms.LCMS_ERROR_ABORT
LCMS_ERROR_SHOW = _lcms.LCMS_ERROR_SHOW
LCMS_ERROR_IGNORE = _lcms.LCMS_ERROR_IGNORE
cmsErrorAction = _lcms.cmsErrorAction
LCMS_ERRC_WARNING = _lcms.LCMS_ERRC_WARNING
LCMS_ERRC_RECOVERABLE = _lcms.LCMS_ERRC_RECOVERABLE
LCMS_ERRC_ABORTED = _lcms.LCMS_ERRC_ABORTED
cmsSetErrorHandler = _lcms.cmsSetErrorHandler
cmsAllocLUT = _lcms.cmsAllocLUT
cmsAllocLinearTable = _lcms.cmsAllocLinearTable
cmsAlloc3DGrid = _lcms.cmsAlloc3DGrid
cmsSetMatrixLUT = _lcms.cmsSetMatrixLUT
cmsSetMatrixLUT4 = _lcms.cmsSetMatrixLUT4
cmsFreeLUT = _lcms.cmsFreeLUT
cmsEvalLUT = _lcms.cmsEvalLUT
cmsEvalLUTreverse = _lcms.cmsEvalLUTreverse
cmsReadICCLut = _lcms.cmsReadICCLut
cmsDupLUT = _lcms.cmsDupLUT
SAMPLER_INSPECT = _lcms.SAMPLER_INSPECT
cmsSample3DGrid = _lcms.cmsSample3DGrid
cmsSetUserFormatters = _lcms.cmsSetUserFormatters
cmsGetUserFormatters = _lcms.cmsGetUserFormatters
cmsIT8Alloc = _lcms.cmsIT8Alloc
cmsIT8Free = _lcms.cmsIT8Free
cmsIT8TableCount = _lcms.cmsIT8TableCount
cmsIT8SetTable = _lcms.cmsIT8SetTable
cmsIT8LoadFromFile = _lcms.cmsIT8LoadFromFile
cmsIT8LoadFromMem = _lcms.cmsIT8LoadFromMem
cmsIT8SaveToFile = _lcms.cmsIT8SaveToFile
cmsIT8SaveToMem = _lcms.cmsIT8SaveToMem
cmsIT8GetSheetType = _lcms.cmsIT8GetSheetType
cmsIT8SetSheetType = _lcms.cmsIT8SetSheetType
cmsIT8SetComment = _lcms.cmsIT8SetComment
cmsIT8SetPropertyStr = _lcms.cmsIT8SetPropertyStr
cmsIT8SetPropertyDbl = _lcms.cmsIT8SetPropertyDbl
cmsIT8SetPropertyHex = _lcms.cmsIT8SetPropertyHex
cmsIT8SetPropertyUncooked = _lcms.cmsIT8SetPropertyUncooked
cmsIT8GetProperty = _lcms.cmsIT8GetProperty
cmsIT8GetPropertyDbl = _lcms.cmsIT8GetPropertyDbl
cmsIT8EnumProperties = _lcms.cmsIT8EnumProperties
cmsIT8GetDataRowCol = _lcms.cmsIT8GetDataRowCol
cmsIT8GetDataRowColDbl = _lcms.cmsIT8GetDataRowColDbl
cmsIT8SetDataRowCol = _lcms.cmsIT8SetDataRowCol
cmsIT8SetDataRowColDbl = _lcms.cmsIT8SetDataRowColDbl
cmsIT8GetData = _lcms.cmsIT8GetData
cmsIT8GetDataDbl = _lcms.cmsIT8GetDataDbl
cmsIT8SetData = _lcms.cmsIT8SetData
cmsIT8SetDataDbl = _lcms.cmsIT8SetDataDbl
cmsIT8GetDataFormat = _lcms.cmsIT8GetDataFormat
cmsIT8SetDataFormat = _lcms.cmsIT8SetDataFormat
cmsIT8EnumDataFormat = _lcms.cmsIT8EnumDataFormat
cmsIT8GetPatchName = _lcms.cmsIT8GetPatchName
cmsIT8SetTableByLabel = _lcms.cmsIT8SetTableByLabel
cmsIT8DefineDblFormat = _lcms.cmsIT8DefineDblFormat
cmsLabEncoded2Float = _lcms.cmsLabEncoded2Float
cmsLabEncoded2Float4 = _lcms.cmsLabEncoded2Float4
cmsFloat2LabEncoded = _lcms.cmsFloat2LabEncoded
cmsFloat2LabEncoded4 = _lcms.cmsFloat2LabEncoded4
cmsXYZEncoded2Float = _lcms.cmsXYZEncoded2Float
cmsFloat2XYZEncoded = _lcms.cmsFloat2XYZEncoded
_cmsAddTextTag = _lcms._cmsAddTextTag
_cmsAddXYZTag = _lcms._cmsAddXYZTag
_cmsAddLUTTag = _lcms._cmsAddLUTTag
_cmsAddGammaTag = _lcms._cmsAddGammaTag
_cmsAddChromaticityTag = _lcms._cmsAddChromaticityTag
_cmsAddSequenceDescriptionTag = _lcms._cmsAddSequenceDescriptionTag
_cmsAddNamedColorTag = _lcms._cmsAddNamedColorTag
_cmsAddDateTimeTag = _lcms._cmsAddDateTimeTag
_cmsAddColorantTableTag = _lcms._cmsAddColorantTableTag
_cmsQuickFloor = _lcms._cmsQuickFloor
_cmsClampWord = _lcms._cmsClampWord
cmsSignalError = _lcms.cmsSignalError
class _cmsTestAlign16(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, _cmsTestAlign16, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, _cmsTestAlign16, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a"] = _lcms._cmsTestAlign16_a_set
    __swig_getmethods__["a"] = _lcms._cmsTestAlign16_a_get
    if _newclass:a = property(_lcms._cmsTestAlign16_a_get, _lcms._cmsTestAlign16_a_set)
    __swig_setmethods__["b"] = _lcms._cmsTestAlign16_b_set
    __swig_getmethods__["b"] = _lcms._cmsTestAlign16_b_get
    if _newclass:b = property(_lcms._cmsTestAlign16_b_get, _lcms._cmsTestAlign16_b_set)
    def __init__(self, *args): 
        this = _lcms.new__cmsTestAlign16(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete__cmsTestAlign16
    __del__ = lambda self : None;
_cmsTestAlign16_swigregister = _lcms._cmsTestAlign16_swigregister
_cmsTestAlign16_swigregister(_cmsTestAlign16)

class _cmsTestAlign8(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, _cmsTestAlign8, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, _cmsTestAlign8, name)
    __repr__ = _swig_repr
    __swig_setmethods__["a"] = _lcms._cmsTestAlign8_a_set
    __swig_getmethods__["a"] = _lcms._cmsTestAlign8_a_get
    if _newclass:a = property(_lcms._cmsTestAlign8_a_get, _lcms._cmsTestAlign8_a_set)
    __swig_setmethods__["b"] = _lcms._cmsTestAlign8_b_set
    __swig_getmethods__["b"] = _lcms._cmsTestAlign8_b_get
    if _newclass:b = property(_lcms._cmsTestAlign8_b_get, _lcms._cmsTestAlign8_b_set)
    def __init__(self, *args): 
        this = _lcms.new__cmsTestAlign8(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete__cmsTestAlign8
    __del__ = lambda self : None;
_cmsTestAlign8_swigregister = _lcms._cmsTestAlign8_swigregister
_cmsTestAlign8_swigregister(_cmsTestAlign8)

FixedMul = _lcms.FixedMul
FixedSquare = _lcms.FixedSquare
ToFixedDomain = _lcms.ToFixedDomain
FromFixedDomain = _lcms.FromFixedDomain
FixedLERP = _lcms.FixedLERP
FixedScale = _lcms.FixedScale
VX = _lcms.VX
VY = _lcms.VY
VZ = _lcms.VZ
class WVEC3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, WVEC3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, WVEC3, name)
    __repr__ = _swig_repr
    __swig_setmethods__["n"] = _lcms.WVEC3_n_set
    __swig_getmethods__["n"] = _lcms.WVEC3_n_get
    if _newclass:n = property(_lcms.WVEC3_n_get, _lcms.WVEC3_n_set)
    def __init__(self, *args): 
        this = _lcms.new_WVEC3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_WVEC3
    __del__ = lambda self : None;
WVEC3_swigregister = _lcms.WVEC3_swigregister
WVEC3_swigregister(WVEC3)

class WMAT3(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, WMAT3, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, WMAT3, name)
    __repr__ = _swig_repr
    __swig_setmethods__["v"] = _lcms.WMAT3_v_set
    __swig_getmethods__["v"] = _lcms.WMAT3_v_get
    if _newclass:v = property(_lcms.WMAT3_v_get, _lcms.WMAT3_v_set)
    def __init__(self, *args): 
        this = _lcms.new_WMAT3(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_WMAT3
    __del__ = lambda self : None;
WMAT3_swigregister = _lcms.WMAT3_swigregister
WMAT3_swigregister(WMAT3)

VEC3init = _lcms.VEC3init
VEC3initF = _lcms.VEC3initF
VEC3toFix = _lcms.VEC3toFix
VEC3fromFix = _lcms.VEC3fromFix
VEC3scaleFix = _lcms.VEC3scaleFix
VEC3swap = _lcms.VEC3swap
VEC3divK = _lcms.VEC3divK
VEC3perK = _lcms.VEC3perK
VEC3minus = _lcms.VEC3minus
VEC3perComp = _lcms.VEC3perComp
VEC3equal = _lcms.VEC3equal
VEC3equalF = _lcms.VEC3equalF
VEC3scaleAndCut = _lcms.VEC3scaleAndCut
VEC3cross = _lcms.VEC3cross
VEC3saturate = _lcms.VEC3saturate
VEC3distance = _lcms.VEC3distance
VEC3length = _lcms.VEC3length
MAT3identity = _lcms.MAT3identity
MAT3per = _lcms.MAT3per
MAT3perK = _lcms.MAT3perK
MAT3inverse = _lcms.MAT3inverse
MAT3solve = _lcms.MAT3solve
MAT3det = _lcms.MAT3det
MAT3eval = _lcms.MAT3eval
MAT3toFix = _lcms.MAT3toFix
MAT3fromFix = _lcms.MAT3fromFix
MAT3evalW = _lcms.MAT3evalW
MAT3isIdentity = _lcms.MAT3isIdentity
MAT3scaleAndCut = _lcms.MAT3scaleAndCut
cmsIsLinear = _lcms.cmsIsLinear
class L8PARAMS(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, L8PARAMS, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, L8PARAMS, name)
    __repr__ = _swig_repr
    __swig_setmethods__["X0"] = _lcms.L8PARAMS_X0_set
    __swig_getmethods__["X0"] = _lcms.L8PARAMS_X0_get
    if _newclass:X0 = property(_lcms.L8PARAMS_X0_get, _lcms.L8PARAMS_X0_set)
    __swig_setmethods__["Y0"] = _lcms.L8PARAMS_Y0_set
    __swig_getmethods__["Y0"] = _lcms.L8PARAMS_Y0_get
    if _newclass:Y0 = property(_lcms.L8PARAMS_Y0_get, _lcms.L8PARAMS_Y0_set)
    __swig_setmethods__["Z0"] = _lcms.L8PARAMS_Z0_set
    __swig_getmethods__["Z0"] = _lcms.L8PARAMS_Z0_get
    if _newclass:Z0 = property(_lcms.L8PARAMS_Z0_get, _lcms.L8PARAMS_Z0_set)
    __swig_setmethods__["rx"] = _lcms.L8PARAMS_rx_set
    __swig_getmethods__["rx"] = _lcms.L8PARAMS_rx_get
    if _newclass:rx = property(_lcms.L8PARAMS_rx_get, _lcms.L8PARAMS_rx_set)
    __swig_setmethods__["ry"] = _lcms.L8PARAMS_ry_set
    __swig_getmethods__["ry"] = _lcms.L8PARAMS_ry_get
    if _newclass:ry = property(_lcms.L8PARAMS_ry_get, _lcms.L8PARAMS_ry_set)
    __swig_setmethods__["rz"] = _lcms.L8PARAMS_rz_set
    __swig_getmethods__["rz"] = _lcms.L8PARAMS_rz_get
    if _newclass:rz = property(_lcms.L8PARAMS_rz_get, _lcms.L8PARAMS_rz_set)
    def __init__(self, *args): 
        this = _lcms.new_L8PARAMS(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_L8PARAMS
    __del__ = lambda self : None;
L8PARAMS_swigregister = _lcms.L8PARAMS_swigregister
L8PARAMS_swigregister(L8PARAMS)

class L16PARAMS(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, L16PARAMS, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, L16PARAMS, name)
    __repr__ = _swig_repr
    __swig_setmethods__["nSamples"] = _lcms.L16PARAMS_nSamples_set
    __swig_getmethods__["nSamples"] = _lcms.L16PARAMS_nSamples_get
    if _newclass:nSamples = property(_lcms.L16PARAMS_nSamples_get, _lcms.L16PARAMS_nSamples_set)
    __swig_setmethods__["nInputs"] = _lcms.L16PARAMS_nInputs_set
    __swig_getmethods__["nInputs"] = _lcms.L16PARAMS_nInputs_get
    if _newclass:nInputs = property(_lcms.L16PARAMS_nInputs_get, _lcms.L16PARAMS_nInputs_set)
    __swig_setmethods__["nOutputs"] = _lcms.L16PARAMS_nOutputs_set
    __swig_getmethods__["nOutputs"] = _lcms.L16PARAMS_nOutputs_get
    if _newclass:nOutputs = property(_lcms.L16PARAMS_nOutputs_get, _lcms.L16PARAMS_nOutputs_set)
    __swig_setmethods__["Domain"] = _lcms.L16PARAMS_Domain_set
    __swig_getmethods__["Domain"] = _lcms.L16PARAMS_Domain_get
    if _newclass:Domain = property(_lcms.L16PARAMS_Domain_get, _lcms.L16PARAMS_Domain_set)
    __swig_setmethods__["opta1"] = _lcms.L16PARAMS_opta1_set
    __swig_getmethods__["opta1"] = _lcms.L16PARAMS_opta1_get
    if _newclass:opta1 = property(_lcms.L16PARAMS_opta1_get, _lcms.L16PARAMS_opta1_set)
    __swig_setmethods__["opta2"] = _lcms.L16PARAMS_opta2_set
    __swig_getmethods__["opta2"] = _lcms.L16PARAMS_opta2_get
    if _newclass:opta2 = property(_lcms.L16PARAMS_opta2_get, _lcms.L16PARAMS_opta2_set)
    __swig_setmethods__["opta3"] = _lcms.L16PARAMS_opta3_set
    __swig_getmethods__["opta3"] = _lcms.L16PARAMS_opta3_get
    if _newclass:opta3 = property(_lcms.L16PARAMS_opta3_get, _lcms.L16PARAMS_opta3_set)
    __swig_setmethods__["opta4"] = _lcms.L16PARAMS_opta4_set
    __swig_getmethods__["opta4"] = _lcms.L16PARAMS_opta4_get
    if _newclass:opta4 = property(_lcms.L16PARAMS_opta4_get, _lcms.L16PARAMS_opta4_set)
    __swig_setmethods__["opta5"] = _lcms.L16PARAMS_opta5_set
    __swig_getmethods__["opta5"] = _lcms.L16PARAMS_opta5_get
    if _newclass:opta5 = property(_lcms.L16PARAMS_opta5_get, _lcms.L16PARAMS_opta5_set)
    __swig_setmethods__["opta6"] = _lcms.L16PARAMS_opta6_set
    __swig_getmethods__["opta6"] = _lcms.L16PARAMS_opta6_get
    if _newclass:opta6 = property(_lcms.L16PARAMS_opta6_get, _lcms.L16PARAMS_opta6_set)
    __swig_setmethods__["opta7"] = _lcms.L16PARAMS_opta7_set
    __swig_getmethods__["opta7"] = _lcms.L16PARAMS_opta7_get
    if _newclass:opta7 = property(_lcms.L16PARAMS_opta7_get, _lcms.L16PARAMS_opta7_set)
    __swig_setmethods__["opta8"] = _lcms.L16PARAMS_opta8_set
    __swig_getmethods__["opta8"] = _lcms.L16PARAMS_opta8_get
    if _newclass:opta8 = property(_lcms.L16PARAMS_opta8_get, _lcms.L16PARAMS_opta8_set)
    __swig_setmethods__["Interp3D"] = _lcms.L16PARAMS_Interp3D_set
    __swig_getmethods__["Interp3D"] = _lcms.L16PARAMS_Interp3D_get
    if _newclass:Interp3D = property(_lcms.L16PARAMS_Interp3D_get, _lcms.L16PARAMS_Interp3D_set)
    __swig_setmethods__["p8"] = _lcms.L16PARAMS_p8_set
    __swig_getmethods__["p8"] = _lcms.L16PARAMS_p8_get
    if _newclass:p8 = property(_lcms.L16PARAMS_p8_get, _lcms.L16PARAMS_p8_set)
    def __init__(self, *args): 
        this = _lcms.new_L16PARAMS(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_L16PARAMS
    __del__ = lambda self : None;
L16PARAMS_swigregister = _lcms.L16PARAMS_swigregister
L16PARAMS_swigregister(L16PARAMS)

cmsCalcL16Params = _lcms.cmsCalcL16Params
cmsCalcCLUT16Params = _lcms.cmsCalcCLUT16Params
cmsCalcCLUT16ParamsEx = _lcms.cmsCalcCLUT16ParamsEx
cmsLinearInterpLUT16 = _lcms.cmsLinearInterpLUT16
cmsLinearInterpFixed = _lcms.cmsLinearInterpFixed
cmsReverseLinearInterpLUT16 = _lcms.cmsReverseLinearInterpLUT16
cmsTrilinearInterp16 = _lcms.cmsTrilinearInterp16
cmsTetrahedralInterp16 = _lcms.cmsTetrahedralInterp16
cmsTetrahedralInterp8 = _lcms.cmsTetrahedralInterp8
LUT_HASMATRIX = _lcms.LUT_HASMATRIX
LUT_HASTL1 = _lcms.LUT_HASTL1
LUT_HASTL2 = _lcms.LUT_HASTL2
LUT_HAS3DGRID = _lcms.LUT_HAS3DGRID
LUT_HASMATRIX3 = _lcms.LUT_HASMATRIX3
LUT_HASMATRIX4 = _lcms.LUT_HASMATRIX4
LUT_HASTL3 = _lcms.LUT_HASTL3
LUT_HASTL4 = _lcms.LUT_HASTL4
LUT_V4_OUTPUT_EMULATE_V2 = _lcms.LUT_V4_OUTPUT_EMULATE_V2
LUT_V4_INPUT_EMULATE_V2 = _lcms.LUT_V4_INPUT_EMULATE_V2
LUT_V2_OUTPUT_EMULATE_V4 = _lcms.LUT_V2_OUTPUT_EMULATE_V4
LUT_V2_INPUT_EMULATE_V4 = _lcms.LUT_V2_INPUT_EMULATE_V4
class _lcms_LUT_struc(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, _lcms_LUT_struc, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, _lcms_LUT_struc, name)
    __repr__ = _swig_repr
    __swig_setmethods__["wFlags"] = _lcms._lcms_LUT_struc_wFlags_set
    __swig_getmethods__["wFlags"] = _lcms._lcms_LUT_struc_wFlags_get
    if _newclass:wFlags = property(_lcms._lcms_LUT_struc_wFlags_get, _lcms._lcms_LUT_struc_wFlags_set)
    __swig_setmethods__["Matrix"] = _lcms._lcms_LUT_struc_Matrix_set
    __swig_getmethods__["Matrix"] = _lcms._lcms_LUT_struc_Matrix_get
    if _newclass:Matrix = property(_lcms._lcms_LUT_struc_Matrix_get, _lcms._lcms_LUT_struc_Matrix_set)
    __swig_setmethods__["InputChan"] = _lcms._lcms_LUT_struc_InputChan_set
    __swig_getmethods__["InputChan"] = _lcms._lcms_LUT_struc_InputChan_get
    if _newclass:InputChan = property(_lcms._lcms_LUT_struc_InputChan_get, _lcms._lcms_LUT_struc_InputChan_set)
    __swig_setmethods__["OutputChan"] = _lcms._lcms_LUT_struc_OutputChan_set
    __swig_getmethods__["OutputChan"] = _lcms._lcms_LUT_struc_OutputChan_get
    if _newclass:OutputChan = property(_lcms._lcms_LUT_struc_OutputChan_get, _lcms._lcms_LUT_struc_OutputChan_set)
    __swig_setmethods__["InputEntries"] = _lcms._lcms_LUT_struc_InputEntries_set
    __swig_getmethods__["InputEntries"] = _lcms._lcms_LUT_struc_InputEntries_get
    if _newclass:InputEntries = property(_lcms._lcms_LUT_struc_InputEntries_get, _lcms._lcms_LUT_struc_InputEntries_set)
    __swig_setmethods__["OutputEntries"] = _lcms._lcms_LUT_struc_OutputEntries_set
    __swig_getmethods__["OutputEntries"] = _lcms._lcms_LUT_struc_OutputEntries_get
    if _newclass:OutputEntries = property(_lcms._lcms_LUT_struc_OutputEntries_get, _lcms._lcms_LUT_struc_OutputEntries_set)
    __swig_setmethods__["cLutPoints"] = _lcms._lcms_LUT_struc_cLutPoints_set
    __swig_getmethods__["cLutPoints"] = _lcms._lcms_LUT_struc_cLutPoints_get
    if _newclass:cLutPoints = property(_lcms._lcms_LUT_struc_cLutPoints_get, _lcms._lcms_LUT_struc_cLutPoints_set)
    __swig_setmethods__["L1"] = _lcms._lcms_LUT_struc_L1_set
    __swig_getmethods__["L1"] = _lcms._lcms_LUT_struc_L1_get
    if _newclass:L1 = property(_lcms._lcms_LUT_struc_L1_get, _lcms._lcms_LUT_struc_L1_set)
    __swig_setmethods__["L2"] = _lcms._lcms_LUT_struc_L2_set
    __swig_getmethods__["L2"] = _lcms._lcms_LUT_struc_L2_get
    if _newclass:L2 = property(_lcms._lcms_LUT_struc_L2_get, _lcms._lcms_LUT_struc_L2_set)
    __swig_setmethods__["T"] = _lcms._lcms_LUT_struc_T_set
    __swig_getmethods__["T"] = _lcms._lcms_LUT_struc_T_get
    if _newclass:T = property(_lcms._lcms_LUT_struc_T_get, _lcms._lcms_LUT_struc_T_set)
    __swig_setmethods__["Tsize"] = _lcms._lcms_LUT_struc_Tsize_set
    __swig_getmethods__["Tsize"] = _lcms._lcms_LUT_struc_Tsize_get
    if _newclass:Tsize = property(_lcms._lcms_LUT_struc_Tsize_get, _lcms._lcms_LUT_struc_Tsize_set)
    __swig_setmethods__["In16params"] = _lcms._lcms_LUT_struc_In16params_set
    __swig_getmethods__["In16params"] = _lcms._lcms_LUT_struc_In16params_get
    if _newclass:In16params = property(_lcms._lcms_LUT_struc_In16params_get, _lcms._lcms_LUT_struc_In16params_set)
    __swig_setmethods__["Out16params"] = _lcms._lcms_LUT_struc_Out16params_set
    __swig_getmethods__["Out16params"] = _lcms._lcms_LUT_struc_Out16params_get
    if _newclass:Out16params = property(_lcms._lcms_LUT_struc_Out16params_get, _lcms._lcms_LUT_struc_Out16params_set)
    __swig_setmethods__["CLut16params"] = _lcms._lcms_LUT_struc_CLut16params_set
    __swig_getmethods__["CLut16params"] = _lcms._lcms_LUT_struc_CLut16params_get
    if _newclass:CLut16params = property(_lcms._lcms_LUT_struc_CLut16params_get, _lcms._lcms_LUT_struc_CLut16params_set)
    __swig_setmethods__["Intent"] = _lcms._lcms_LUT_struc_Intent_set
    __swig_getmethods__["Intent"] = _lcms._lcms_LUT_struc_Intent_get
    if _newclass:Intent = property(_lcms._lcms_LUT_struc_Intent_get, _lcms._lcms_LUT_struc_Intent_set)
    __swig_setmethods__["Mat3"] = _lcms._lcms_LUT_struc_Mat3_set
    __swig_getmethods__["Mat3"] = _lcms._lcms_LUT_struc_Mat3_get
    if _newclass:Mat3 = property(_lcms._lcms_LUT_struc_Mat3_get, _lcms._lcms_LUT_struc_Mat3_set)
    __swig_setmethods__["Ofs3"] = _lcms._lcms_LUT_struc_Ofs3_set
    __swig_getmethods__["Ofs3"] = _lcms._lcms_LUT_struc_Ofs3_get
    if _newclass:Ofs3 = property(_lcms._lcms_LUT_struc_Ofs3_get, _lcms._lcms_LUT_struc_Ofs3_set)
    __swig_setmethods__["L3"] = _lcms._lcms_LUT_struc_L3_set
    __swig_getmethods__["L3"] = _lcms._lcms_LUT_struc_L3_get
    if _newclass:L3 = property(_lcms._lcms_LUT_struc_L3_get, _lcms._lcms_LUT_struc_L3_set)
    __swig_setmethods__["L3params"] = _lcms._lcms_LUT_struc_L3params_set
    __swig_getmethods__["L3params"] = _lcms._lcms_LUT_struc_L3params_get
    if _newclass:L3params = property(_lcms._lcms_LUT_struc_L3params_get, _lcms._lcms_LUT_struc_L3params_set)
    __swig_setmethods__["L3Entries"] = _lcms._lcms_LUT_struc_L3Entries_set
    __swig_getmethods__["L3Entries"] = _lcms._lcms_LUT_struc_L3Entries_get
    if _newclass:L3Entries = property(_lcms._lcms_LUT_struc_L3Entries_get, _lcms._lcms_LUT_struc_L3Entries_set)
    __swig_setmethods__["Mat4"] = _lcms._lcms_LUT_struc_Mat4_set
    __swig_getmethods__["Mat4"] = _lcms._lcms_LUT_struc_Mat4_get
    if _newclass:Mat4 = property(_lcms._lcms_LUT_struc_Mat4_get, _lcms._lcms_LUT_struc_Mat4_set)
    __swig_setmethods__["Ofs4"] = _lcms._lcms_LUT_struc_Ofs4_set
    __swig_getmethods__["Ofs4"] = _lcms._lcms_LUT_struc_Ofs4_get
    if _newclass:Ofs4 = property(_lcms._lcms_LUT_struc_Ofs4_get, _lcms._lcms_LUT_struc_Ofs4_set)
    __swig_setmethods__["L4"] = _lcms._lcms_LUT_struc_L4_set
    __swig_getmethods__["L4"] = _lcms._lcms_LUT_struc_L4_get
    if _newclass:L4 = property(_lcms._lcms_LUT_struc_L4_get, _lcms._lcms_LUT_struc_L4_set)
    __swig_setmethods__["L4params"] = _lcms._lcms_LUT_struc_L4params_set
    __swig_getmethods__["L4params"] = _lcms._lcms_LUT_struc_L4params_get
    if _newclass:L4params = property(_lcms._lcms_LUT_struc_L4params_get, _lcms._lcms_LUT_struc_L4params_set)
    __swig_setmethods__["L4Entries"] = _lcms._lcms_LUT_struc_L4Entries_set
    __swig_getmethods__["L4Entries"] = _lcms._lcms_LUT_struc_L4Entries_get
    if _newclass:L4Entries = property(_lcms._lcms_LUT_struc_L4Entries_get, _lcms._lcms_LUT_struc_L4Entries_set)
    __swig_setmethods__["FixGrayAxes"] = _lcms._lcms_LUT_struc_FixGrayAxes_set
    __swig_getmethods__["FixGrayAxes"] = _lcms._lcms_LUT_struc_FixGrayAxes_get
    if _newclass:FixGrayAxes = property(_lcms._lcms_LUT_struc_FixGrayAxes_get, _lcms._lcms_LUT_struc_FixGrayAxes_set)
    __swig_setmethods__["LCurvesSeed"] = _lcms._lcms_LUT_struc_LCurvesSeed_set
    __swig_getmethods__["LCurvesSeed"] = _lcms._lcms_LUT_struc_LCurvesSeed_get
    if _newclass:LCurvesSeed = property(_lcms._lcms_LUT_struc_LCurvesSeed_get, _lcms._lcms_LUT_struc_LCurvesSeed_set)
    def __init__(self, *args): 
        this = _lcms.new__lcms_LUT_struc(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete__lcms_LUT_struc
    __del__ = lambda self : None;
_lcms_LUT_struc_swigregister = _lcms._lcms_LUT_struc_swigregister
_lcms_LUT_struc_swigregister(_lcms_LUT_struc)

_cmsSmoothEndpoints = _lcms._cmsSmoothEndpoints
_cmsCrc32OfGammaTable = _lcms._cmsCrc32OfGammaTable
cmsAllocSampledCurve = _lcms.cmsAllocSampledCurve
cmsFreeSampledCurve = _lcms.cmsFreeSampledCurve
cmsDupSampledCurve = _lcms.cmsDupSampledCurve
cmsConvertGammaToSampledCurve = _lcms.cmsConvertGammaToSampledCurve
cmsConvertSampledCurveToGamma = _lcms.cmsConvertSampledCurveToGamma
cmsEndpointsOfSampledCurve = _lcms.cmsEndpointsOfSampledCurve
cmsClampSampledCurve = _lcms.cmsClampSampledCurve
cmsSmoothSampledCurve = _lcms.cmsSmoothSampledCurve
cmsRescaleSampledCurve = _lcms.cmsRescaleSampledCurve
cmsJoinSampledCurves = _lcms.cmsJoinSampledCurves
MATSHAPER_HASMATRIX = _lcms.MATSHAPER_HASMATRIX
MATSHAPER_HASSHAPER = _lcms.MATSHAPER_HASSHAPER
MATSHAPER_INPUT = _lcms.MATSHAPER_INPUT
MATSHAPER_OUTPUT = _lcms.MATSHAPER_OUTPUT
MATSHAPER_HASINPSHAPER = _lcms.MATSHAPER_HASINPSHAPER
MATSHAPER_ALLSMELTED = _lcms.MATSHAPER_ALLSMELTED
class MATSHAPER(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MATSHAPER, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MATSHAPER, name)
    __repr__ = _swig_repr
    __swig_setmethods__["dwFlags"] = _lcms.MATSHAPER_dwFlags_set
    __swig_getmethods__["dwFlags"] = _lcms.MATSHAPER_dwFlags_get
    if _newclass:dwFlags = property(_lcms.MATSHAPER_dwFlags_get, _lcms.MATSHAPER_dwFlags_set)
    __swig_setmethods__["Matrix"] = _lcms.MATSHAPER_Matrix_set
    __swig_getmethods__["Matrix"] = _lcms.MATSHAPER_Matrix_get
    if _newclass:Matrix = property(_lcms.MATSHAPER_Matrix_get, _lcms.MATSHAPER_Matrix_set)
    __swig_setmethods__["p16"] = _lcms.MATSHAPER_p16_set
    __swig_getmethods__["p16"] = _lcms.MATSHAPER_p16_get
    if _newclass:p16 = property(_lcms.MATSHAPER_p16_get, _lcms.MATSHAPER_p16_set)
    __swig_setmethods__["L"] = _lcms.MATSHAPER_L_set
    __swig_getmethods__["L"] = _lcms.MATSHAPER_L_get
    if _newclass:L = property(_lcms.MATSHAPER_L_get, _lcms.MATSHAPER_L_set)
    __swig_setmethods__["p2_16"] = _lcms.MATSHAPER_p2_16_set
    __swig_getmethods__["p2_16"] = _lcms.MATSHAPER_p2_16_get
    if _newclass:p2_16 = property(_lcms.MATSHAPER_p2_16_get, _lcms.MATSHAPER_p2_16_set)
    __swig_setmethods__["L2"] = _lcms.MATSHAPER_L2_set
    __swig_getmethods__["L2"] = _lcms.MATSHAPER_L2_get
    if _newclass:L2 = property(_lcms.MATSHAPER_L2_get, _lcms.MATSHAPER_L2_set)
    def __init__(self, *args): 
        this = _lcms.new_MATSHAPER(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_MATSHAPER
    __del__ = lambda self : None;
MATSHAPER_swigregister = _lcms.MATSHAPER_swigregister
MATSHAPER_swigregister(MATSHAPER)

cmsAllocMatShaper = _lcms.cmsAllocMatShaper
cmsAllocMatShaper2 = _lcms.cmsAllocMatShaper2
cmsFreeMatShaper = _lcms.cmsFreeMatShaper
cmsEvalMatShaper = _lcms.cmsEvalMatShaper
cmsReadICCMatrixRGB2XYZ = _lcms.cmsReadICCMatrixRGB2XYZ
cmsBuildInputMatrixShaper = _lcms.cmsBuildInputMatrixShaper
cmsBuildOutputMatrixShaper = _lcms.cmsBuildOutputMatrixShaper
cmsAdaptationMatrix = _lcms.cmsAdaptationMatrix
cmsAdaptMatrixToD50 = _lcms.cmsAdaptMatrixToD50
cmsAdaptMatrixFromD50 = _lcms.cmsAdaptMatrixFromD50
cmsReadChromaticAdaptationMatrix = _lcms.cmsReadChromaticAdaptationMatrix
cmsXYZ2LabEncoded = _lcms.cmsXYZ2LabEncoded
cmsLab2XYZEncoded = _lcms.cmsLab2XYZEncoded
_cmsIdentifyWhitePoint = _lcms._cmsIdentifyWhitePoint
_cmsQuantizeVal = _lcms._cmsQuantizeVal
cmsAllocNamedColorList = _lcms.cmsAllocNamedColorList
cmsReadICCnamedColorList = _lcms.cmsReadICCnamedColorList
cmsFreeNamedColorList = _lcms.cmsFreeNamedColorList
cmsAppendNamedColor = _lcms.cmsAppendNamedColor
MAX_TABLE_TAG = _lcms.MAX_TABLE_TAG
class LCMSICCPROFILE(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, LCMSICCPROFILE, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, LCMSICCPROFILE, name)
    __repr__ = _swig_repr
    __swig_setmethods__["stream"] = _lcms.LCMSICCPROFILE_stream_set
    __swig_getmethods__["stream"] = _lcms.LCMSICCPROFILE_stream_get
    if _newclass:stream = property(_lcms.LCMSICCPROFILE_stream_get, _lcms.LCMSICCPROFILE_stream_set)
    __swig_setmethods__["DeviceClass"] = _lcms.LCMSICCPROFILE_DeviceClass_set
    __swig_getmethods__["DeviceClass"] = _lcms.LCMSICCPROFILE_DeviceClass_get
    if _newclass:DeviceClass = property(_lcms.LCMSICCPROFILE_DeviceClass_get, _lcms.LCMSICCPROFILE_DeviceClass_set)
    __swig_setmethods__["ColorSpace"] = _lcms.LCMSICCPROFILE_ColorSpace_set
    __swig_getmethods__["ColorSpace"] = _lcms.LCMSICCPROFILE_ColorSpace_get
    if _newclass:ColorSpace = property(_lcms.LCMSICCPROFILE_ColorSpace_get, _lcms.LCMSICCPROFILE_ColorSpace_set)
    __swig_setmethods__["PCS"] = _lcms.LCMSICCPROFILE_PCS_set
    __swig_getmethods__["PCS"] = _lcms.LCMSICCPROFILE_PCS_get
    if _newclass:PCS = property(_lcms.LCMSICCPROFILE_PCS_get, _lcms.LCMSICCPROFILE_PCS_set)
    __swig_setmethods__["RenderingIntent"] = _lcms.LCMSICCPROFILE_RenderingIntent_set
    __swig_getmethods__["RenderingIntent"] = _lcms.LCMSICCPROFILE_RenderingIntent_get
    if _newclass:RenderingIntent = property(_lcms.LCMSICCPROFILE_RenderingIntent_get, _lcms.LCMSICCPROFILE_RenderingIntent_set)
    __swig_setmethods__["flags"] = _lcms.LCMSICCPROFILE_flags_set
    __swig_getmethods__["flags"] = _lcms.LCMSICCPROFILE_flags_get
    if _newclass:flags = property(_lcms.LCMSICCPROFILE_flags_get, _lcms.LCMSICCPROFILE_flags_set)
    __swig_setmethods__["attributes"] = _lcms.LCMSICCPROFILE_attributes_set
    __swig_getmethods__["attributes"] = _lcms.LCMSICCPROFILE_attributes_get
    if _newclass:attributes = property(_lcms.LCMSICCPROFILE_attributes_get, _lcms.LCMSICCPROFILE_attributes_set)
    __swig_setmethods__["Illuminant"] = _lcms.LCMSICCPROFILE_Illuminant_set
    __swig_getmethods__["Illuminant"] = _lcms.LCMSICCPROFILE_Illuminant_get
    if _newclass:Illuminant = property(_lcms.LCMSICCPROFILE_Illuminant_get, _lcms.LCMSICCPROFILE_Illuminant_set)
    __swig_setmethods__["Version"] = _lcms.LCMSICCPROFILE_Version_set
    __swig_getmethods__["Version"] = _lcms.LCMSICCPROFILE_Version_get
    if _newclass:Version = property(_lcms.LCMSICCPROFILE_Version_get, _lcms.LCMSICCPROFILE_Version_set)
    __swig_setmethods__["ChromaticAdaptation"] = _lcms.LCMSICCPROFILE_ChromaticAdaptation_set
    __swig_getmethods__["ChromaticAdaptation"] = _lcms.LCMSICCPROFILE_ChromaticAdaptation_get
    if _newclass:ChromaticAdaptation = property(_lcms.LCMSICCPROFILE_ChromaticAdaptation_get, _lcms.LCMSICCPROFILE_ChromaticAdaptation_set)
    __swig_setmethods__["MediaWhitePoint"] = _lcms.LCMSICCPROFILE_MediaWhitePoint_set
    __swig_getmethods__["MediaWhitePoint"] = _lcms.LCMSICCPROFILE_MediaWhitePoint_get
    if _newclass:MediaWhitePoint = property(_lcms.LCMSICCPROFILE_MediaWhitePoint_get, _lcms.LCMSICCPROFILE_MediaWhitePoint_set)
    __swig_setmethods__["MediaBlackPoint"] = _lcms.LCMSICCPROFILE_MediaBlackPoint_set
    __swig_getmethods__["MediaBlackPoint"] = _lcms.LCMSICCPROFILE_MediaBlackPoint_get
    if _newclass:MediaBlackPoint = property(_lcms.LCMSICCPROFILE_MediaBlackPoint_get, _lcms.LCMSICCPROFILE_MediaBlackPoint_set)
    __swig_setmethods__["ProfileID"] = _lcms.LCMSICCPROFILE_ProfileID_set
    __swig_getmethods__["ProfileID"] = _lcms.LCMSICCPROFILE_ProfileID_get
    if _newclass:ProfileID = property(_lcms.LCMSICCPROFILE_ProfileID_get, _lcms.LCMSICCPROFILE_ProfileID_set)
    __swig_setmethods__["TagCount"] = _lcms.LCMSICCPROFILE_TagCount_set
    __swig_getmethods__["TagCount"] = _lcms.LCMSICCPROFILE_TagCount_get
    if _newclass:TagCount = property(_lcms.LCMSICCPROFILE_TagCount_get, _lcms.LCMSICCPROFILE_TagCount_set)
    __swig_setmethods__["TagNames"] = _lcms.LCMSICCPROFILE_TagNames_set
    __swig_getmethods__["TagNames"] = _lcms.LCMSICCPROFILE_TagNames_get
    if _newclass:TagNames = property(_lcms.LCMSICCPROFILE_TagNames_get, _lcms.LCMSICCPROFILE_TagNames_set)
    __swig_setmethods__["TagSizes"] = _lcms.LCMSICCPROFILE_TagSizes_set
    __swig_getmethods__["TagSizes"] = _lcms.LCMSICCPROFILE_TagSizes_get
    if _newclass:TagSizes = property(_lcms.LCMSICCPROFILE_TagSizes_get, _lcms.LCMSICCPROFILE_TagSizes_set)
    __swig_setmethods__["TagOffsets"] = _lcms.LCMSICCPROFILE_TagOffsets_set
    __swig_getmethods__["TagOffsets"] = _lcms.LCMSICCPROFILE_TagOffsets_get
    if _newclass:TagOffsets = property(_lcms.LCMSICCPROFILE_TagOffsets_get, _lcms.LCMSICCPROFILE_TagOffsets_set)
    __swig_setmethods__["TagPtrs"] = _lcms.LCMSICCPROFILE_TagPtrs_set
    __swig_getmethods__["TagPtrs"] = _lcms.LCMSICCPROFILE_TagPtrs_get
    if _newclass:TagPtrs = property(_lcms.LCMSICCPROFILE_TagPtrs_get, _lcms.LCMSICCPROFILE_TagPtrs_set)
    __swig_setmethods__["PhysicalFile"] = _lcms.LCMSICCPROFILE_PhysicalFile_set
    __swig_getmethods__["PhysicalFile"] = _lcms.LCMSICCPROFILE_PhysicalFile_get
    if _newclass:PhysicalFile = property(_lcms.LCMSICCPROFILE_PhysicalFile_get, _lcms.LCMSICCPROFILE_PhysicalFile_set)
    __swig_setmethods__["IsWrite"] = _lcms.LCMSICCPROFILE_IsWrite_set
    __swig_getmethods__["IsWrite"] = _lcms.LCMSICCPROFILE_IsWrite_get
    if _newclass:IsWrite = property(_lcms.LCMSICCPROFILE_IsWrite_get, _lcms.LCMSICCPROFILE_IsWrite_set)
    __swig_setmethods__["SaveAs8Bits"] = _lcms.LCMSICCPROFILE_SaveAs8Bits_set
    __swig_getmethods__["SaveAs8Bits"] = _lcms.LCMSICCPROFILE_SaveAs8Bits_get
    if _newclass:SaveAs8Bits = property(_lcms.LCMSICCPROFILE_SaveAs8Bits_get, _lcms.LCMSICCPROFILE_SaveAs8Bits_set)
    __swig_setmethods__["Created"] = _lcms.LCMSICCPROFILE_Created_set
    __swig_getmethods__["Created"] = _lcms.LCMSICCPROFILE_Created_get
    if _newclass:Created = property(_lcms.LCMSICCPROFILE_Created_get, _lcms.LCMSICCPROFILE_Created_set)
    __swig_setmethods__["Read"] = _lcms.LCMSICCPROFILE_Read_set
    __swig_getmethods__["Read"] = _lcms.LCMSICCPROFILE_Read_get
    if _newclass:Read = property(_lcms.LCMSICCPROFILE_Read_get, _lcms.LCMSICCPROFILE_Read_set)
    __swig_setmethods__["Seek"] = _lcms.LCMSICCPROFILE_Seek_set
    __swig_getmethods__["Seek"] = _lcms.LCMSICCPROFILE_Seek_get
    if _newclass:Seek = property(_lcms.LCMSICCPROFILE_Seek_get, _lcms.LCMSICCPROFILE_Seek_set)
    __swig_setmethods__["Close"] = _lcms.LCMSICCPROFILE_Close_set
    __swig_getmethods__["Close"] = _lcms.LCMSICCPROFILE_Close_get
    if _newclass:Close = property(_lcms.LCMSICCPROFILE_Close_get, _lcms.LCMSICCPROFILE_Close_set)
    __swig_setmethods__["Tell"] = _lcms.LCMSICCPROFILE_Tell_set
    __swig_getmethods__["Tell"] = _lcms.LCMSICCPROFILE_Tell_get
    if _newclass:Tell = property(_lcms.LCMSICCPROFILE_Tell_get, _lcms.LCMSICCPROFILE_Tell_set)
    __swig_setmethods__["Write"] = _lcms.LCMSICCPROFILE_Write_set
    __swig_getmethods__["Write"] = _lcms.LCMSICCPROFILE_Write_get
    if _newclass:Write = property(_lcms.LCMSICCPROFILE_Write_get, _lcms.LCMSICCPROFILE_Write_set)
    __swig_setmethods__["UsedSpace"] = _lcms.LCMSICCPROFILE_UsedSpace_set
    __swig_getmethods__["UsedSpace"] = _lcms.LCMSICCPROFILE_UsedSpace_get
    if _newclass:UsedSpace = property(_lcms.LCMSICCPROFILE_UsedSpace_get, _lcms.LCMSICCPROFILE_UsedSpace_set)
    def __init__(self, *args): 
        this = _lcms.new_LCMSICCPROFILE(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_LCMSICCPROFILE
    __del__ = lambda self : None;
LCMSICCPROFILE_swigregister = _lcms.LCMSICCPROFILE_swigregister
LCMSICCPROFILE_swigregister(LCMSICCPROFILE)

_cmsCreateProfilePlaceholder = _lcms._cmsCreateProfilePlaceholder
_cmsSearchTag = _lcms._cmsSearchTag
_cmsInitTag = _lcms._cmsInitTag
_cmsCreateProfileFromFilePlaceholder = _lcms._cmsCreateProfileFromFilePlaceholder
_cmsCreateProfileFromMemPlaceholder = _lcms._cmsCreateProfileFromMemPlaceholder
_cmsSetSaveToDisk = _lcms._cmsSetSaveToDisk
_cmsSetSaveToMemory = _lcms._cmsSetSaveToMemory
class _cmsTRANSFORM(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, _cmsTRANSFORM, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, _cmsTRANSFORM, name)
    __repr__ = _swig_repr
    __swig_setmethods__["InputFormat"] = _lcms._cmsTRANSFORM_InputFormat_set
    __swig_getmethods__["InputFormat"] = _lcms._cmsTRANSFORM_InputFormat_get
    if _newclass:InputFormat = property(_lcms._cmsTRANSFORM_InputFormat_get, _lcms._cmsTRANSFORM_InputFormat_set)
    __swig_setmethods__["OutputFormat"] = _lcms._cmsTRANSFORM_OutputFormat_set
    __swig_getmethods__["OutputFormat"] = _lcms._cmsTRANSFORM_OutputFormat_get
    if _newclass:OutputFormat = property(_lcms._cmsTRANSFORM_OutputFormat_get, _lcms._cmsTRANSFORM_OutputFormat_set)
    __swig_setmethods__["StrideIn"] = _lcms._cmsTRANSFORM_StrideIn_set
    __swig_getmethods__["StrideIn"] = _lcms._cmsTRANSFORM_StrideIn_get
    if _newclass:StrideIn = property(_lcms._cmsTRANSFORM_StrideIn_get, _lcms._cmsTRANSFORM_StrideIn_set)
    __swig_setmethods__["StrideOut"] = _lcms._cmsTRANSFORM_StrideOut_set
    __swig_getmethods__["StrideOut"] = _lcms._cmsTRANSFORM_StrideOut_get
    if _newclass:StrideOut = property(_lcms._cmsTRANSFORM_StrideOut_get, _lcms._cmsTRANSFORM_StrideOut_set)
    __swig_setmethods__["Intent"] = _lcms._cmsTRANSFORM_Intent_set
    __swig_getmethods__["Intent"] = _lcms._cmsTRANSFORM_Intent_get
    if _newclass:Intent = property(_lcms._cmsTRANSFORM_Intent_get, _lcms._cmsTRANSFORM_Intent_set)
    __swig_setmethods__["ProofIntent"] = _lcms._cmsTRANSFORM_ProofIntent_set
    __swig_getmethods__["ProofIntent"] = _lcms._cmsTRANSFORM_ProofIntent_get
    if _newclass:ProofIntent = property(_lcms._cmsTRANSFORM_ProofIntent_get, _lcms._cmsTRANSFORM_ProofIntent_set)
    __swig_setmethods__["DoGamutCheck"] = _lcms._cmsTRANSFORM_DoGamutCheck_set
    __swig_getmethods__["DoGamutCheck"] = _lcms._cmsTRANSFORM_DoGamutCheck_get
    if _newclass:DoGamutCheck = property(_lcms._cmsTRANSFORM_DoGamutCheck_get, _lcms._cmsTRANSFORM_DoGamutCheck_set)
    __swig_setmethods__["InputProfile"] = _lcms._cmsTRANSFORM_InputProfile_set
    __swig_getmethods__["InputProfile"] = _lcms._cmsTRANSFORM_InputProfile_get
    if _newclass:InputProfile = property(_lcms._cmsTRANSFORM_InputProfile_get, _lcms._cmsTRANSFORM_InputProfile_set)
    __swig_setmethods__["OutputProfile"] = _lcms._cmsTRANSFORM_OutputProfile_set
    __swig_getmethods__["OutputProfile"] = _lcms._cmsTRANSFORM_OutputProfile_get
    if _newclass:OutputProfile = property(_lcms._cmsTRANSFORM_OutputProfile_get, _lcms._cmsTRANSFORM_OutputProfile_set)
    __swig_setmethods__["PreviewProfile"] = _lcms._cmsTRANSFORM_PreviewProfile_set
    __swig_getmethods__["PreviewProfile"] = _lcms._cmsTRANSFORM_PreviewProfile_get
    if _newclass:PreviewProfile = property(_lcms._cmsTRANSFORM_PreviewProfile_get, _lcms._cmsTRANSFORM_PreviewProfile_set)
    __swig_setmethods__["EntryColorSpace"] = _lcms._cmsTRANSFORM_EntryColorSpace_set
    __swig_getmethods__["EntryColorSpace"] = _lcms._cmsTRANSFORM_EntryColorSpace_get
    if _newclass:EntryColorSpace = property(_lcms._cmsTRANSFORM_EntryColorSpace_get, _lcms._cmsTRANSFORM_EntryColorSpace_set)
    __swig_setmethods__["ExitColorSpace"] = _lcms._cmsTRANSFORM_ExitColorSpace_set
    __swig_getmethods__["ExitColorSpace"] = _lcms._cmsTRANSFORM_ExitColorSpace_get
    if _newclass:ExitColorSpace = property(_lcms._cmsTRANSFORM_ExitColorSpace_get, _lcms._cmsTRANSFORM_ExitColorSpace_set)
    __swig_setmethods__["dwOriginalFlags"] = _lcms._cmsTRANSFORM_dwOriginalFlags_set
    __swig_getmethods__["dwOriginalFlags"] = _lcms._cmsTRANSFORM_dwOriginalFlags_get
    if _newclass:dwOriginalFlags = property(_lcms._cmsTRANSFORM_dwOriginalFlags_get, _lcms._cmsTRANSFORM_dwOriginalFlags_set)
    __swig_setmethods__["m1"] = _lcms._cmsTRANSFORM_m1_set
    __swig_getmethods__["m1"] = _lcms._cmsTRANSFORM_m1_get
    if _newclass:m1 = property(_lcms._cmsTRANSFORM_m1_get, _lcms._cmsTRANSFORM_m1_set)
    __swig_setmethods__["m2"] = _lcms._cmsTRANSFORM_m2_set
    __swig_getmethods__["m2"] = _lcms._cmsTRANSFORM_m2_get
    if _newclass:m2 = property(_lcms._cmsTRANSFORM_m2_get, _lcms._cmsTRANSFORM_m2_set)
    __swig_setmethods__["of1"] = _lcms._cmsTRANSFORM_of1_set
    __swig_getmethods__["of1"] = _lcms._cmsTRANSFORM_of1_get
    if _newclass:of1 = property(_lcms._cmsTRANSFORM_of1_get, _lcms._cmsTRANSFORM_of1_set)
    __swig_setmethods__["of2"] = _lcms._cmsTRANSFORM_of2_set
    __swig_getmethods__["of2"] = _lcms._cmsTRANSFORM_of2_get
    if _newclass:of2 = property(_lcms._cmsTRANSFORM_of2_get, _lcms._cmsTRANSFORM_of2_set)
    __swig_setmethods__["xform"] = _lcms._cmsTRANSFORM_xform_set
    __swig_getmethods__["xform"] = _lcms._cmsTRANSFORM_xform_get
    if _newclass:xform = property(_lcms._cmsTRANSFORM_xform_get, _lcms._cmsTRANSFORM_xform_set)
    __swig_setmethods__["FromInput"] = _lcms._cmsTRANSFORM_FromInput_set
    __swig_getmethods__["FromInput"] = _lcms._cmsTRANSFORM_FromInput_get
    if _newclass:FromInput = property(_lcms._cmsTRANSFORM_FromInput_get, _lcms._cmsTRANSFORM_FromInput_set)
    __swig_setmethods__["FromDevice"] = _lcms._cmsTRANSFORM_FromDevice_set
    __swig_getmethods__["FromDevice"] = _lcms._cmsTRANSFORM_FromDevice_get
    if _newclass:FromDevice = property(_lcms._cmsTRANSFORM_FromDevice_get, _lcms._cmsTRANSFORM_FromDevice_set)
    __swig_setmethods__["Stage1"] = _lcms._cmsTRANSFORM_Stage1_set
    __swig_getmethods__["Stage1"] = _lcms._cmsTRANSFORM_Stage1_get
    if _newclass:Stage1 = property(_lcms._cmsTRANSFORM_Stage1_get, _lcms._cmsTRANSFORM_Stage1_set)
    __swig_setmethods__["Stage2"] = _lcms._cmsTRANSFORM_Stage2_set
    __swig_getmethods__["Stage2"] = _lcms._cmsTRANSFORM_Stage2_get
    if _newclass:Stage2 = property(_lcms._cmsTRANSFORM_Stage2_get, _lcms._cmsTRANSFORM_Stage2_set)
    __swig_setmethods__["ToDevice"] = _lcms._cmsTRANSFORM_ToDevice_set
    __swig_getmethods__["ToDevice"] = _lcms._cmsTRANSFORM_ToDevice_get
    if _newclass:ToDevice = property(_lcms._cmsTRANSFORM_ToDevice_get, _lcms._cmsTRANSFORM_ToDevice_set)
    __swig_setmethods__["ToOutput"] = _lcms._cmsTRANSFORM_ToOutput_set
    __swig_getmethods__["ToOutput"] = _lcms._cmsTRANSFORM_ToOutput_get
    if _newclass:ToOutput = property(_lcms._cmsTRANSFORM_ToOutput_get, _lcms._cmsTRANSFORM_ToOutput_set)
    __swig_setmethods__["Device2PCS"] = _lcms._cmsTRANSFORM_Device2PCS_set
    __swig_getmethods__["Device2PCS"] = _lcms._cmsTRANSFORM_Device2PCS_get
    if _newclass:Device2PCS = property(_lcms._cmsTRANSFORM_Device2PCS_get, _lcms._cmsTRANSFORM_Device2PCS_set)
    __swig_setmethods__["PCS2Device"] = _lcms._cmsTRANSFORM_PCS2Device_set
    __swig_getmethods__["PCS2Device"] = _lcms._cmsTRANSFORM_PCS2Device_get
    if _newclass:PCS2Device = property(_lcms._cmsTRANSFORM_PCS2Device_get, _lcms._cmsTRANSFORM_PCS2Device_set)
    __swig_setmethods__["Gamut"] = _lcms._cmsTRANSFORM_Gamut_set
    __swig_getmethods__["Gamut"] = _lcms._cmsTRANSFORM_Gamut_get
    if _newclass:Gamut = property(_lcms._cmsTRANSFORM_Gamut_get, _lcms._cmsTRANSFORM_Gamut_set)
    __swig_setmethods__["Preview"] = _lcms._cmsTRANSFORM_Preview_set
    __swig_getmethods__["Preview"] = _lcms._cmsTRANSFORM_Preview_get
    if _newclass:Preview = property(_lcms._cmsTRANSFORM_Preview_get, _lcms._cmsTRANSFORM_Preview_set)
    __swig_setmethods__["DeviceLink"] = _lcms._cmsTRANSFORM_DeviceLink_set
    __swig_getmethods__["DeviceLink"] = _lcms._cmsTRANSFORM_DeviceLink_get
    if _newclass:DeviceLink = property(_lcms._cmsTRANSFORM_DeviceLink_get, _lcms._cmsTRANSFORM_DeviceLink_set)
    __swig_setmethods__["GamutCheck"] = _lcms._cmsTRANSFORM_GamutCheck_set
    __swig_getmethods__["GamutCheck"] = _lcms._cmsTRANSFORM_GamutCheck_get
    if _newclass:GamutCheck = property(_lcms._cmsTRANSFORM_GamutCheck_get, _lcms._cmsTRANSFORM_GamutCheck_set)
    __swig_setmethods__["InMatShaper"] = _lcms._cmsTRANSFORM_InMatShaper_set
    __swig_getmethods__["InMatShaper"] = _lcms._cmsTRANSFORM_InMatShaper_get
    if _newclass:InMatShaper = property(_lcms._cmsTRANSFORM_InMatShaper_get, _lcms._cmsTRANSFORM_InMatShaper_set)
    __swig_setmethods__["OutMatShaper"] = _lcms._cmsTRANSFORM_OutMatShaper_set
    __swig_getmethods__["OutMatShaper"] = _lcms._cmsTRANSFORM_OutMatShaper_get
    if _newclass:OutMatShaper = property(_lcms._cmsTRANSFORM_OutMatShaper_get, _lcms._cmsTRANSFORM_OutMatShaper_set)
    __swig_setmethods__["SmeltMatShaper"] = _lcms._cmsTRANSFORM_SmeltMatShaper_set
    __swig_getmethods__["SmeltMatShaper"] = _lcms._cmsTRANSFORM_SmeltMatShaper_get
    if _newclass:SmeltMatShaper = property(_lcms._cmsTRANSFORM_SmeltMatShaper_get, _lcms._cmsTRANSFORM_SmeltMatShaper_set)
    __swig_setmethods__["Phase1"] = _lcms._cmsTRANSFORM_Phase1_set
    __swig_getmethods__["Phase1"] = _lcms._cmsTRANSFORM_Phase1_get
    if _newclass:Phase1 = property(_lcms._cmsTRANSFORM_Phase1_get, _lcms._cmsTRANSFORM_Phase1_set)
    __swig_setmethods__["Phase2"] = _lcms._cmsTRANSFORM_Phase2_set
    __swig_getmethods__["Phase2"] = _lcms._cmsTRANSFORM_Phase2_get
    if _newclass:Phase2 = property(_lcms._cmsTRANSFORM_Phase2_get, _lcms._cmsTRANSFORM_Phase2_set)
    __swig_setmethods__["Phase3"] = _lcms._cmsTRANSFORM_Phase3_set
    __swig_getmethods__["Phase3"] = _lcms._cmsTRANSFORM_Phase3_get
    if _newclass:Phase3 = property(_lcms._cmsTRANSFORM_Phase3_get, _lcms._cmsTRANSFORM_Phase3_set)
    __swig_setmethods__["NamedColorList"] = _lcms._cmsTRANSFORM_NamedColorList_set
    __swig_getmethods__["NamedColorList"] = _lcms._cmsTRANSFORM_NamedColorList_get
    if _newclass:NamedColorList = property(_lcms._cmsTRANSFORM_NamedColorList_get, _lcms._cmsTRANSFORM_NamedColorList_set)
    __swig_setmethods__["lInputV4Lab"] = _lcms._cmsTRANSFORM_lInputV4Lab_set
    __swig_getmethods__["lInputV4Lab"] = _lcms._cmsTRANSFORM_lInputV4Lab_get
    if _newclass:lInputV4Lab = property(_lcms._cmsTRANSFORM_lInputV4Lab_get, _lcms._cmsTRANSFORM_lInputV4Lab_set)
    __swig_setmethods__["lOutputV4Lab"] = _lcms._cmsTRANSFORM_lOutputV4Lab_set
    __swig_getmethods__["lOutputV4Lab"] = _lcms._cmsTRANSFORM_lOutputV4Lab_get
    if _newclass:lOutputV4Lab = property(_lcms._cmsTRANSFORM_lOutputV4Lab_get, _lcms._cmsTRANSFORM_lOutputV4Lab_set)
    __swig_setmethods__["CacheIn"] = _lcms._cmsTRANSFORM_CacheIn_set
    __swig_getmethods__["CacheIn"] = _lcms._cmsTRANSFORM_CacheIn_get
    if _newclass:CacheIn = property(_lcms._cmsTRANSFORM_CacheIn_get, _lcms._cmsTRANSFORM_CacheIn_set)
    __swig_setmethods__["CacheOut"] = _lcms._cmsTRANSFORM_CacheOut_set
    __swig_getmethods__["CacheOut"] = _lcms._cmsTRANSFORM_CacheOut_get
    if _newclass:CacheOut = property(_lcms._cmsTRANSFORM_CacheOut_get, _lcms._cmsTRANSFORM_CacheOut_set)
    __swig_setmethods__["AdaptationState"] = _lcms._cmsTRANSFORM_AdaptationState_set
    __swig_getmethods__["AdaptationState"] = _lcms._cmsTRANSFORM_AdaptationState_get
    if _newclass:AdaptationState = property(_lcms._cmsTRANSFORM_AdaptationState_get, _lcms._cmsTRANSFORM_AdaptationState_set)
    __swig_setmethods__["rwlock"] = _lcms._cmsTRANSFORM_rwlock_set
    __swig_getmethods__["rwlock"] = _lcms._cmsTRANSFORM_rwlock_get
    if _newclass:rwlock = property(_lcms._cmsTRANSFORM_rwlock_get, _lcms._cmsTRANSFORM_rwlock_set)
    def __init__(self, *args): 
        this = _lcms.new__cmsTRANSFORM(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete__cmsTRANSFORM
    __del__ = lambda self : None;
_cmsTRANSFORM_swigregister = _lcms._cmsTRANSFORM_swigregister
_cmsTRANSFORM_swigregister(_cmsTRANSFORM)

_cmsIdentifyInputFormat = _lcms._cmsIdentifyInputFormat
_cmsIdentifyOutputFormat = _lcms._cmsIdentifyOutputFormat
XYZRel = _lcms.XYZRel
LabRel = _lcms.LabRel
cmsChooseCnvrt = _lcms.cmsChooseCnvrt
_cmsEndPointsBySpace = _lcms._cmsEndPointsBySpace
_cmsWhiteBySpace = _lcms._cmsWhiteBySpace
Clamp_L = _lcms.Clamp_L
Clamp_ab = _lcms.Clamp_ab
LCMS_BPFLAGS_D50_ADAPTED = _lcms.LCMS_BPFLAGS_D50_ADAPTED
cmsDetectBlackPoint = _lcms.cmsDetectBlackPoint
_cmsReasonableGridpointsByColorspace = _lcms._cmsReasonableGridpointsByColorspace
_cmsPrecalculateDeviceLink = _lcms._cmsPrecalculateDeviceLink
_cmsPrecalculateBlackPreservingDeviceLink = _lcms._cmsPrecalculateBlackPreservingDeviceLink
_cmsPrecalculateGamutCheck = _lcms._cmsPrecalculateGamutCheck
_cmsFixWhiteMisalignment = _lcms._cmsFixWhiteMisalignment
_cmsBlessLUT8 = _lcms._cmsBlessLUT8
_cmsComputeGamutLUT = _lcms._cmsComputeGamutLUT
_cmsComputeSoftProofLUT = _lcms._cmsComputeSoftProofLUT
_cmsComputePrelinearizationTablesFromXFORM = _lcms._cmsComputePrelinearizationTablesFromXFORM
_cmsBuildKToneCurve = _lcms._cmsBuildKToneCurve
class icTagSignature(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, icTagSignature, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, icTagSignature, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _lcms.new_icTagSignature(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_icTagSignature
    __del__ = lambda self : None;
icTagSignature_swigregister = _lcms.icTagSignature_swigregister
icTagSignature_swigregister(icTagSignature)

class StrPointer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, StrPointer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, StrPointer, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _lcms.new_StrPointer(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lcms.delete_StrPointer
    __del__ = lambda self : None;
StrPointer_swigregister = _lcms.StrPointer_swigregister
StrPointer_swigregister(StrPointer)



