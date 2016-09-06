#include "SocketApiTypes.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QTextStream>

namespace SaleaeSocketApi
{
class SaleaeClient : public QObject
{

public:
    SaleaeClient(QString host_str = "127.0.0.1", int port_input = 10429 , QObject *parent = NULL);

    const bool PrintCommandsToConsole = false;

    QTcpSocket Socket;
    QTextStream Stream;
    int port;
    QString host;

    //Command strings
    const QString set_trigger_cmd = "SET_TRIGGER";
    const QString set_num_samples_cmd = "SET_NUM_SAMPLES";
    const QString get_sample_rate_cmd = "GET_SAMPLE_RATE";
    const QString set_sample_rate_cmd = "SET_SAMPLE_RATE";

    const QString set_capture_seconds_cmd = "SET_CAPTURE_SECONDS";
    const QString capture_to_file_cmd = "CAPTURE_TO_FILE";
    const QString save_to_file_cmd = "SAVE_TO_FILE";
    const QString load_from_file_cmd = "LOAD_FROM_FILE";
    const QString export_data_cmd = "EXPORT_DATA";
    const QString export_data2_cmd = "EXPORT_DATA2";

    const QString get_all_sample_rates_cmd = "GET_ALL_SAMPLE_RATES";
    const QString get_analyzers_cmd = "GET_ANALYZERS";
    const QString export_analyzer_cmd = "EXPORT_ANALYZER";
    const QString get_inputs_cmd = "GET_INPUTS";
    const QString capture_cmd = "CAPTURE";
    const QString stop_capture_cmd = "STOP_CAPTURE";
    const QString get_capture_pretrigger_buffer_size_cmd = "GET_CAPTURE_PRETRIGGER_BUFFER_SIZE";
    const QString set_capture_pretrigger_buffer_size_cmd = "SET_CAPTURE_PRETRIGGER_BUFFER_SIZE";
    const QString get_connected_devices_cmd = "GET_CONNECTED_DEVICES";
    const QString select_active_device_cmd = "SELECT_ACTIVE_DEVICE";

    const QString get_active_channels_cmd = "GET_ACTIVE_CHANNELS";
    const QString set_active_channels_cmd = "SET_ACTIVE_CHANNELS";
    const QString reset_active_channels_cmd = "RESET_ACTIVE_CHANNELS";

    const QString set_performance_cmd = "SET_PERFORMANCE";
    const QString get_performance_cmd = "GET_PERFORMANCE";
    const QString is_processing_complete_cmd = "IS_PROCESSING_COMPLETE";
    const QString is_analyzer_complete_cmd = "IS_ANALYZER_COMPLETE";

    const QString set_digital_voltage_option_cmd = "SET_DIGITAL_VOLTAGE_OPTION";
    const QString get_digital_voltage_options_cmd = "GET_DIGITAL_VOLTAGE_OPTIONS";

    const QString close_all_tabs_cmd = "CLOSE_ALL_TABS";

    enum DeviceType
    {
        Logic,
        Logic16,
        Logic4,
        Logic8,
        LogicPro8,
        LogicPro16
    };

    enum Trigger {

        None,

        High,

        Low,

        FallingEdge,

        RisingEdge,

        NegativePulse,

        PositivePulse
    };
    Q_ENUM(Trigger)

    enum PerformanceOption { OneHundredPercent = 100, EightyPercent = 80, SixtyPercent = 60, FortyPercent = 40, TwentyPercent = 20 };

    //Export Data
    enum DataExportChannelSelection
    {
        AllChannels,
        SpecificChannels
    };
    Q_ENUM(DataExportChannelSelection)


    enum DataExportMixedModeExportType
    {
        DigitalOnly,
        AnalogOnly,
        AnalogAndDigital
    };
    Q_ENUM(DataExportMixedModeExportType)

    enum DataExportSampleRangeType {
        RangeAll,
        RangeTimes
    };
    Q_ENUM(DataExportSampleRangeType)

    enum DataExportType {
        ExportBinary,
        ExportCsv,
        ExportVcd,
        ExportMatlab
    };
    Q_ENUM(DataExportType)

    enum CsvHeadersType
    {
        CsvIncludesHeaders,
        CsvNoHeaders
    };
    Q_ENUM(CsvHeadersType)

    enum CsvDelimiterType
    {
        CsvComma,
        CsvTab
    };
    Q_ENUM(CsvDelimiterType)

    enum CsvOutputMode
    {
        CsvSingleNumber,
        CsvOneColumnPerBit
    };
    Q_ENUM(CsvOutputMode)

    enum CsvTimestampType
    {
        CsvTime,
        CsvSample
    };
    Q_ENUM(CsvTimestampType)

    enum CsvBase
    {
        CsvBinary,
        CsvDecimal,
        CsvHexadecimal,
        CsvAscii
    };
    Q_ENUM(CsvBase)

    enum CsvDensity
    {
        CsvTransition,
        CsvComplete
    };
    Q_ENUM(CsvDensity)

    enum BinaryOutputMode
    {
        BinaryEverySample,
        BinaryEveryChange
    };
    Q_ENUM(BinaryOutputMode)

    enum BinaryBitShifting
    {
        BinaryOriginalBitPositions,
        BinaryShiftRight
    };
    Q_ENUM(BinaryBitShifting)

    enum BinaryOutputWordSize {
        Binary8Bit = 8,
        Binary16Bit = 16,
        Binary32Bit = 32,
        Binary64Bit = 64
    };
    Q_ENUM(BinaryOutputWordSize)

    enum AnalogOutputFormat
    {
        Voltage,
        ADC
    };
    Q_ENUM(AnalogOutputFormat)

    struct ConnectedDevice
    {
        DeviceType type;// { get; set; }
        QString Name;// { get; set; }
        quint64 DeviceId;// { get; set; }
        int Index;// { get; set; }
        bool IsActive;// { get; set; }
    };

    struct Channel
    {
        enum ChannelDataType
        {
            AnalogChannel,
            DigitalChannel
        };

        int Index;// { get; set; }
        ChannelDataType DataType;// { get; set; }

        QString GetExportstring()
        {
            QString ret = QString::number(Index);
            switch (DataType) {
            case AnalogChannel:
                ret.append("AnalogChannel");
                break;
            case DigitalChannel:
                ret.append("DigitalChannel"); // FIXME use MOC
                break;
            }
            return ret;
        }

    };

    struct DigitalVoltageOption
    {
        int Index;
        QString Description;
        bool IsSelected;
    };

    struct ExportDataStruct
    {
        /// <summary>
        /// The fully qualified path to the target file. Folder must exist. Path must be absolute. Always required
        /// features like ~/ and %appdata% are not supported. You can exand those first before passing the path.
        /// </summary>
        QString FileName;

        /// <summary>
        /// This option is only required & applied IF your capture contains digital and analog channels, AND you select "Specific Channels" for the channel selection option.
        /// i. e. If you then select digital only, you will get the native, digital only interface for an export mode.
        /// </summary>
        DataExportMixedModeExportType DataExportMixedExportMode;

        /// <summary>
        /// This option allows you to export all channels in the capture, or only specific channels. Alaways required
        /// </summary>
        DataExportChannelSelection ExportChannelSelection;
        /// <summary>
        /// List channel indexes of digital channels to export. Only required if you select "SpecificChannels"
        /// </summary>
        QList<int> DigitalChannelsToExport;
        /// <summary>
        /// List channel indexes of analog channels to export. Only required if you select "SpecificChannels"
        /// </summary>
        QList<int> AnalogChannelsToExport;

        /// <summary>
        /// Export all time, or just export a specific range of time. Always required
        /// Command will NAK if a custom time range extends past captured data.
        /// </summary>
        DataExportSampleRangeType SamplesRangeType; //{ RangeAll, RangeTimes }
        /// <summary>
        /// Start time of export. Only appies if "RangeTimes" is set.
        /// Relative to trigger sample, can be negative.
        /// </summary>
        double StartingTime;
        /// <summary>
        /// End time of export. Only appies if "RangeTimes" is set.
        /// Relative to trigger sample, can be negative.
        /// </summary>
        double EndingTime;

        /// <summary>
        /// Primary export type. Always required
        /// </summary>
        DataExportType ExportType; //{ ExportBinary, ExportCsv, ExportVcd }


        /// <summary>
        /// Required for all CSV exports.
        /// </summary>
        CsvHeadersType CsvIncludeHeaders; //{ CsvIncludesHeaders, CsvNoHeaders }
        /// <summary>
        /// Required for all CSV exports
        /// </summary>
        CsvDelimiterType CsvDelimiter;//{ CsvComma, CsvTab }
        /// <summary>
        /// Required only when exporting digital only CSV.
        /// Does not apply when exporting analog and digital or analog only.
        /// </summary>
        CsvOutputMode CsvOutput;//{ CsvSingleNumber, CsvOneColumnPerBit }
        /// <summary>
        /// Only applies when exporting digital only CSV
        /// Does not apply when exporting analog and digital or analog only. (time values used)
        /// </summary>
        CsvTimestampType CsvTimestamp;//{ CsvTime, CsvSample }
        /// <summary>
        /// Required when exporting analog samples as raw ADC value.
        /// Also Required when exporting digital only data in "CsvSingleNumber" format
        /// </summary>
        CsvBase CsvDisplayBase;//{ CsvBinary, CsvDecimal, CsvHexadecimal, CsvAscii }
        /// <summary>
        /// Only required when exporting digital only CSV data.
        /// CsvTransition produces a smaller file where only transition timestamps are exported
        /// CsvComplete includes every single sample
        /// </summary>
        CsvDensity CsvDensityMode;//{ CsvTransition, CsvComplete }


        //Type: Binary
        /// <summary>
        /// Only required for digital only Binary mode
        /// </summary>
        BinaryOutputMode BinaryOutput;//{ BinaryEverySample, BinaryEveryChange }
        /// <summary>
        /// Only required for digital only Binary mode
        /// </summary>
        BinaryBitShifting BinaryBitShiftingMode;//{ BinaryOriginalBitPositions, BinaryShiftRight }
        /// <summary>
        /// Only required for digital only Binary mode
        /// </summary>
        BinaryOutputWordSize BinOutputWordSize;//{ Binary8Bit, Binary16Bit, Binary32Bit, Binary64Bit }

        /// <summary>
        /// ADC values or floating point voltages. Required for all export types that include analog channels
        /// </summary>
        AnalogOutputFormat AnalogFormat; //This feature needs v1.1.32+
    };

    QString CustomCommand( QString export_command );
    void SetTrigger( QList<Trigger> triggers, double minimum_pulse_width_s = 0.0, double maximum_pulse_width_s = 1.0 );
    void SetNumSamples( int num_samples );
    void SetCaptureSeconds( double seconds );

    void SetDigitalVoltageOption( DigitalVoltageOption option );
    QList<DigitalVoltageOption> GetDigitalVoltageOptions();

    void CloseAllTabs();

    QList<SampleRate> GetAvailableSampleRates();
    void SetSampleRate( SampleRate sample_rate );
    SampleRate GetSampleRate();

    void CaptureToFile( QString file );
    void SaveToFile( QString file );
    void LoadFromFile( QString file );

    void ExportData( ExportDataStruct export_data_struct );
    bool ExportData2( ExportDataStruct export_settings, bool capture_contains_digital_channels, bool capture_contains_analog_channels );

    QList<Analyzer> GetAnalyzers();
    void ExportAnalyzers( int selected, QString filename, bool mXmitFile );

    void Capture();
    void StopCapture();

    int GetCapturePretriggerBufferSize();
    void SetCapturePretriggerBufferSize( int buffer_size );

    QList<ConnectedDevice> GetConnectedDevices();

    void SelectActiveDevice( int device_number );

    void SetPerformanceOption( PerformanceOption performance );
    PerformanceOption GetPerformanceOption();

    bool IsProcessingComplete();
    bool IsAnalyzerProcessingComplete( int index );
    bool BlockUntillProcessingCompleteOrTimeout(const quint32 timeout );

    void GetActiveChannels( QList<int> digital_channels, QList<int> analog_channels );
    void SetActiveChannels(QList<int> digital_channels, QList<int> analog_channels );

    void ResetActiveChannels();
private:
    void Writestring(const QString &str );

    void GetResponse( QString &  response );
    bool TryParseDeviceType( QString input, DeviceType & device_type );

};



class QStringHelper
{

public:

    QString Readstring(  QTextStream & stream )
    {
        int max_length = 128;
        QString str = "";
        while( true )
        {
            str.append(stream.read( max_length ));
            if (str.length() > max_length)
                break;
        }
        return str;
    }

    void WriteLine( QString str )
    {
        /*if( SaleaeClient::PrintCommandsToConsole )
            Console.WriteLine( str );*/
    }

    void Write( QString str )
    {
        /*if( SaleaeClient::PrintCommandsToConsole )
            Console.Write( str );*/ // FIXME use QloggingCategory
    }
};


}
