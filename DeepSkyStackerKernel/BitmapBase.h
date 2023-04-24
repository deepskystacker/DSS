#pragma once

class CMemoryBitmap;
class CBitmapCharacteristics;

std::shared_ptr<CMemoryBitmap> CreateBitmap(const CBitmapCharacteristics& bc);
