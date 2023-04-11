#include "stdafx.h"
#include "BitmapInfo.h"
#include "Ztrace.h"

bool RetrieveEXIFInfo(LPCTSTR szFileName, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	auto pBitmap = std::make_unique<Gdiplus::Bitmap>(CComBSTR(szFileName));
	return RetrieveEXIFInfo(pBitmap.get(), BitmapInfo);
}

bool RetrieveEXIFInfo(Gdiplus::Bitmap* pBitmap, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	const auto getExifItem = [pBitmap, &bResult](const PROPID propertyId, const unsigned short type, auto& field) -> void
	{
		const auto dwPropertySize = pBitmap->GetPropertyItemSize(propertyId);
		if (dwPropertySize != 0)
		{
			auto buffer = std::make_unique<std::uint8_t[]>(dwPropertySize);
			Gdiplus::PropertyItem* const propertyItem = reinterpret_cast<Gdiplus::PropertyItem*>(buffer.get());

			if (propertyItem->type == type && pBitmap->GetPropertyItem(propertyId, dwPropertySize, propertyItem) == Gdiplus::Status::Ok)
			{
				if (propertyItem->type == PropertyTagTypeRational)
				{
					const std::uint32_t* pValues = static_cast<std::uint32_t*>(propertyItem->value);
					const std::uint32_t dwNumerator = *pValues;
					const std::uint32_t dwDenominator = *(pValues + 1);
					if (dwDenominator != 0)
					{
						if constexpr (std::is_same_v<decltype(field), double&>)
						{
							field = static_cast<double>(dwNumerator) / static_cast<double>(dwDenominator);
							bResult = true;
						}
					}
				}
				else if (propertyItem->type == PropertyTagTypeShort)
				{
					if constexpr (std::is_same_v<decltype(field), int&>)
					{
						const std::uint16_t* pValue = static_cast<std::uint16_t*>(propertyItem->value);
						field = static_cast<int>(*pValue);
						bResult = true;
					}
				}
				else if (propertyItem->type == PropertyTagTypeASCII)
				{
					if constexpr (std::is_same_v<decltype(field), CString&>)
					{
						field = static_cast<char*>(propertyItem->value);
						bResult = true;
					}
				}
			}
		}
	};

	if (pBitmap != nullptr)
	{
		getExifItem(PropertyTagExifExposureTime, PropertyTagTypeRational, BitmapInfo.m_fExposure);
		getExifItem(PropertyTagExifFNumber, PropertyTagTypeRational, BitmapInfo.m_fAperture);
		getExifItem(PropertyTagExifISOSpeed, PropertyTagTypeShort, BitmapInfo.m_lISOSpeed);

		getExifItem(PropertyTagEquipModel, PropertyTagTypeASCII, BitmapInfo.m_strModel);
		BitmapInfo.m_strModel = BitmapInfo.m_strModel.trimmed();

		CString strDateTime;
		getExifItem(PropertyTagDateTime, PropertyTagTypeASCII, strDateTime);
		// Parse the string : YYYY/MM/DD hh:mm:ss
		//                    0123456789012345678
		BitmapInfo.m_DateTime.wYear = _ttol(strDateTime.Left(4));
		BitmapInfo.m_DateTime.wMonth = _ttol(strDateTime.Mid(5, 2));
		BitmapInfo.m_DateTime.wDay = _ttol(strDateTime.Mid(8, 2));
		BitmapInfo.m_DateTime.wHour = _ttol(strDateTime.Mid(11, 2));
		BitmapInfo.m_DateTime.wMinute = _ttol(strDateTime.Mid(14, 2));
		BitmapInfo.m_DateTime.wSecond = _ttol(strDateTime.Mid(17, 2));

		//UINT dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagExifExposureTime);
		//if (dwPropertySize != 0)
		//{
		//	auto buffer = std::make_unique<std::uint8_t[]>(dwPropertySize);
		//	// PropertyTagTypeRational
		//	Gdiplus::PropertyItem* const propertyItem = reinterpret_cast<Gdiplus::PropertyItem*>(buffer.get());

		//	if (pBitmap->GetPropertyItem(PropertyTagExifExposureTime, dwPropertySize, propertyItem) == Ok)
		//	{
		//		if(propertyItem->type == PropertyTagTypeRational)
		//		{
		//			std::uint32_t* pValues = static_cast<std::uint32_t*>(propertyItem->value);
		//			std::uint32_t dwNumerator, dwDenominator;

		//			dwNumerator = *pValues;
		//			dwDenominator = *(pValues + 1);

		//			if (dwDenominator != 0)
		//			{
		//				BitmapInfo.m_fExposure = static_cast<double>(dwNumerator) / static_cast<double>(dwDenominator);
		//				bResult = true;
		//			};
		//		};
		//	};
		//};

		//dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagExifFNumber);
		//if (dwPropertySize)
		//{
		//	// PropertyTagTypeRational
		//	PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);

		//	if (pBitmap->GetPropertyItem(PropertyTagExifFNumber, dwPropertySize, propertyItem) == Ok)
		//	{
		//		if (propertyItem->type == PropertyTagTypeRational)
		//		{
		//			UINT *			pValues = (UINT*)propertyItem->value;
		//			UINT			dwNumerator,
		//				dwDenominator;

		//			dwNumerator = *pValues;
		//			pValues++;
		//			dwDenominator = *pValues;

		//			if (dwDenominator)
		//			{
		//				BitmapInfo.m_fAperture = (double)dwNumerator / (double)dwDenominator;
		//				bResult = true;
		//			};
		//		};
		//	};

		//	free(propertyItem);
		//};

		//dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagExifISOSpeed);
		//if (dwPropertySize)
		//{
		//	// PropertyTagTypeShort
		//	PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);

		//	if (pBitmap->GetPropertyItem(PropertyTagExifISOSpeed, dwPropertySize, propertyItem) == Ok)
		//	{
		//		if(propertyItem->type == PropertyTagTypeShort)
		//		{
		//			BitmapInfo.m_lISOSpeed = *((WORD*)propertyItem->value);
		//			bResult = true;
		//		};
		//	};

		//	free(propertyItem);
		//};

		//dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagEquipModel);
		//if (dwPropertySize)
		//{
		//	// PropertyTagTypeASCII
		//	PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);
		//	if (pBitmap->GetPropertyItem(PropertyTagEquipModel, dwPropertySize, propertyItem) == Ok)
		//	{
		//		if(propertyItem->type == PropertyTagTypeASCII)
		//		{
		//			BitmapInfo.m_strModel = (char*)propertyItem->value;
		//			BitmapInfo.m_strModel.TrimRight();
		//			BitmapInfo.m_strModel.TrimLeft();
		//			bResult = true;
		//		};
		//	};

		//	free(propertyItem);
		//};

		//dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagDateTime);
		//if (dwPropertySize)
		//{
		//	// PropertyTagTypeASCII
		//	PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);
		//	if (pBitmap->GetPropertyItem(PropertyTagDateTime, dwPropertySize, propertyItem) == Ok)
		//	{
		//		if(propertyItem->type == PropertyTagTypeASCII)
		//		{
		//			CString				strDateTime = (char*)propertyItem->value;

		//			// Parse the string : YYYY/MM/DD hh:mm:ss
		//			//                    0123456789012345678
		//			BitmapInfo.m_DateTime.wYear  = _ttol(strDateTime.Left(4));
		//			BitmapInfo.m_DateTime.wMonth = _ttol(strDateTime.Mid(5, 2));
		//			BitmapInfo.m_DateTime.wDay   = _ttol(strDateTime.Mid(8, 2));
		//			BitmapInfo.m_DateTime.wHour	 = _ttol(strDateTime.Mid(11, 2));
		//			BitmapInfo.m_DateTime.wMinute= _ttol(strDateTime.Mid(14, 2));
		//			BitmapInfo.m_DateTime.wSecond= _ttol(strDateTime.Mid(17, 2));

		//			bResult = true;
		//		};
		//	};

		//	free(propertyItem);
		//};

	};

	return bResult;
}