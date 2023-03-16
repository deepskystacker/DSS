#pragma once

class CBitmapPartFile final
{
public:
	QString m_tempFileName;
	int m_lStartRow;
	int m_lEndRow;
	int m_lWidth{ 0 };
	int m_lNrBitmaps{ 0 };

private:
	void	CopyFrom(const CBitmapPartFile& bp)
	{
		m_tempFileName = bp.m_tempFileName;
		m_lStartRow = bp.m_lStartRow;
		m_lEndRow = bp.m_lEndRow;
		m_lWidth = bp.m_lWidth;
		m_lNrBitmaps = bp.m_lNrBitmaps;
	};

public:
	CBitmapPartFile(QString&& tmpFile, const int lStartRow, const int lEndRow) noexcept :
		m_tempFileName{ std::move(tmpFile) },
		m_lStartRow{ lStartRow },
		m_lEndRow{ lEndRow }
	{};

	CBitmapPartFile(const CBitmapPartFile&) = default;
	CBitmapPartFile(CBitmapPartFile&&) noexcept = default;
	CBitmapPartFile& operator=(const CBitmapPartFile&) = default;
	CBitmapPartFile& operator=(CBitmapPartFile&&) noexcept = default;
	~CBitmapPartFile() = default;
};
