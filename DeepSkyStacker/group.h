#if !defined(GROUP_H)
#define GROUP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <memory>
#include <vector>

#include <QCoreApplication>
#include <QString>

namespace DSS
{
	class Group
	{
	private:
		static uint nextIndex;		// Defined in source file StackingDlg.cpp - initially 0

	public:
		explicit Group::Group() :
			Index(nextIndex++),			// First group is Main Group with Index of 0
			ImageInfo(std::make_shared<LISTBITMAPVECTOR>())
		{
			Index = nextIndex++;
			if (0 == Index)
			{
				Name = QCoreApplication::translate("StackingDlg", "Main Group");
			}
			else
			{
				Name = QCoreApplication::translate("StackingDlg", "Group %1").arg(Index);
			}
		}

		//
		// This is a very simple class so we can let the compiler synthesise the copy ctor 
		// and operator =
		//

		//
		// Accessors
		//
		QString name() const noexcept { return Name; };
		Group& setName(QString const& name) noexcept { Name = name; return *this; };

		uint index() const noexcept { return Index; };

		std::shared_ptr<LISTBITMAPVECTOR> imageInfo() const { return ImageInfo; };

		void addInfo(const CListBitmap& info) { ImageInfo->emplace_back(info); };

	protected:
		//
		// Every group has a name - initially "Main Group" or Group n"
		//
		QString Name;
		std::shared_ptr<LISTBITMAPVECTOR> ImageInfo;
		uint Index;

	};
}
#endif
