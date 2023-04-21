#pragma once
#include "dssbase.h"
#include "DSSCommon.h"
class CMemoryBitmap;

class StackingParams
{
public:
	StackingParams() :
		m_stackingOptions{ 0 },
		m_tiffFormat{ TF_32BITRGBFLOAT },
		m_tiffCompression{ TC_NONE },
		m_terminalOutputMode{ TERMINAL_OUTPUT_MODE::BASIC }
	{
	}
	virtual ~StackingParams() = default;

	enum class eStackingOption : uint32_t
	{
		REGISTER = 1 << 0,
		STACKING = 1 << 1,
		FORCE_REGISTER = 1 << 2,
		SAVE_INTERMEDIATE = 1 << 3,
		SAVE_CALIBRATED = 1 << 4,
		FITS_OUTPUT = 1 << 5,
	};
	using TStackType = std::underlying_type_t <eStackingOption>;

	bool IsOptionSet(eStackingOption option) { return (m_stackingOptions & static_cast<TStackType>(option)); }
	TIFFFORMAT GetTiffFormat() const { return m_tiffFormat; }
	TIFFCOMPRESSION GetTiffCompression() const { return m_tiffCompression; }
	TERMINAL_OUTPUT_MODE GetTerminalMode() const { return m_terminalOutputMode; }
	const QString& GetFileList() const { return m_fileList; }
	const QString& GetOutputFilename() const { return m_outputFile; }

	void SetOption(eStackingOption option) { m_stackingOptions |= static_cast<TStackType>(option); }
	void SetOptions(TStackType options) { m_stackingOptions = options; }
	void SetFileList(const QString& fileList) { m_fileList = fileList; }
	void SetOutputFile(const QString& outputFile) { m_outputFile = outputFile; }
	void SetTiffFormat(const TIFFFORMAT format) { m_tiffFormat = format; }
	void SetTiffCompression(const TIFFCOMPRESSION compression) { m_tiffCompression = compression; }
	void SetOutputStyle(const TERMINAL_OUTPUT_MODE style) { m_terminalOutputMode = style; }

protected:
	TStackType m_stackingOptions;
	TIFFFORMAT m_tiffFormat;
	TIFFCOMPRESSION m_tiffCompression;
	TERMINAL_OUTPUT_MODE m_terminalOutputMode;
	QString m_fileList;
	QString m_outputFile;
};

class StackingTask : public QObject
{
	Q_OBJECT
public:
	StackingTask(QObject* parent, std::function<void(StackingParams&, QTextStream&)> func, StackingParams& params, QTextStream& consoleOut) :
		QObject(parent),
		m_parameters(params),
		m_consoleOut(consoleOut),
		stackFunction(func)
	{}

public slots:
	void run()
	{
		// Do processing here
		if (stackFunction)
			stackFunction(m_parameters, m_consoleOut);
		emit finished();
	}

signals:
	void finished();

protected:
	std::function<void(StackingParams&, QTextStream&)> stackFunction;
	StackingParams& m_parameters;
	QTextStream& m_consoleOut;
};

class DeepSkyStackerCommandLine : 
	public QCoreApplication,
	public DSSBase
{
	Q_OBJECT

public:
	DeepSkyStackerCommandLine(int& argc, char** argv);
	~DeepSkyStackerCommandLine() = default;

	bool Initialise();
	bool Run();

	void reportError(const QString& message, const QString& type, Severity severity, Method method, bool terminate) override;

protected:
	void SetOption(StackingParams::eStackingOption option) { stackingParams.SetOption(option); }
	void SetOptions(StackingParams::TStackType options) { stackingParams.SetOptions(options); }
	QTextStream& ConsoleOut() { return m_consoleOut; }
	StackingParams& GetStackingParams() { return stackingParams; }

	bool DecodeCommandLine();
	void OutputCommandLineHelp();

protected:
	static bool fileExists(const QString& path);
	static bool checkFileCanBeWrittenTo(const QString& path);
	static void SaveBitmap(StackingParams& stackingParams, const std::shared_ptr<CMemoryBitmap> pBitmap);
	static void Process(StackingParams& stackingParams, QTextStream& consoleOut);

protected:
	StackingParams stackingParams;
	QTextStream m_consoleOut;
};