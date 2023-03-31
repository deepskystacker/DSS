#pragma once

class MRUPath
{
private:
	size_t	size;

public:
	QString					mruType;
	std::vector<fs::path>	paths;

	MRUPath(QString type = QString(), size_t sz = 10) :
		size(sz)
	{
		if (type.isEmpty())
			mruType = "FileLists";
		else
			mruType = type;
	}

	void setType(QString type)
	{
		mruType = type;
	}

	MRUPath(const MRUPath&) = delete;	// No copying

	~MRUPath() = default;

	MRUPath& operator = (const MRUPath&) = delete; // No assignment

	void readSettings()
	{
		QSettings			settings;
		uint				count;

		paths.clear();

		QString keyName(mruType);
		keyName += "/NrMRU";

		count = settings.value(keyName, 0).toUInt();

		for (size_t i = 0; i != count; i++)
		{
			keyName = QString("%1/MRU%2")
				.arg(mruType).arg(i);

			fs::path path(settings.value(keyName).toString().toStdU16String());

			if (status(path).type() == fs::file_type::regular)
				paths.push_back(path);
		}
	}

	/* ------------------------------------------------------------------- */

	void saveSettings()
	{
		QSettings	settings;

		QString keyName(mruType);
		keyName += "/NrMRU";

		// Clear all the entries first
		settings.remove(mruType);

		settings.setValue(keyName, (uint)paths.size());
		for (size_t i = 0; i != paths.size(); i++)
		{
			keyName = QString("%1/MRU%2")
				.arg(mruType).arg(i);

			//
			// Convert the path to a string and save in the settings.
			//
			settings.setValue(keyName, QString::fromStdU16String(paths[i].generic_u16string()));
		}
	}


	/* ------------------------------------------------------------------- */

	void Add(const fs::path & path)
	{
		//
		// Is the supplied path already to be found in the mru
		//
		auto it = std::find(paths.begin(), paths.end(), path);

		//
		// If we found the path, delete it from the current location
		//
		if (it != paths.end())
		{
			paths.erase(it);
		}

		//
		// Regardless of whether we already found it
		// insert it at the front
		// 
		paths.insert(paths.begin(), path);

		if (paths.size() > size)
			paths.resize(size);
	}

};
