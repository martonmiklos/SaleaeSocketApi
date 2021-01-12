#ifndef SOCKETAPI_H
#define SOCKETAPI_H

#include "SaleaeSocketApiTypes.h"

#include <QList>
#include <QLoggingCategory>
#include <QMetaEnum>
#include <QObject>
#include <QString>
#include <QTcpSocket>

Q_DECLARE_LOGGING_CATEGORY(saleaeSocketAPI)

namespace SaleaeSocketApi
{
class SaleaeClient : public QObject
{
    Q_OBJECT
public:
    SaleaeClient(QString host_str = "127.0.0.1", int port_input = 10429 , QObject *parent = nullptr);
    // connect and disconnect function names are reserved
    bool connectToLogic(QString host_str = "127.0.0.1", quint16 port_input = 10429);
    void disconnectFromLogic();

    //Command strings
    static QString set_trigger_cmd ;
    static QString set_num_samples_cmd ;
    static QString get_sample_rate_cmd ;
    static QString set_sample_rate_cmd ;

    static QString set_capture_seconds_cmd ;
    static QString capture_to_file_cmd ;
    static QString save_to_file_cmd ;
    static QString load_from_file_cmd ;
    static QString export_data_cmd ;
    static QString export_data2_cmd ;

    static QString get_all_sample_rates_cmd ;
    static QString get_analyzers_cmd ;
    static QString export_analyzer_cmd ;
    static QString get_inputs_cmd ;
    static QString capture_cmd ;
    static QString stop_capture_cmd ;
    static QString get_capture_pretrigger_buffer_size_cmd ;
    static QString set_capture_pretrigger_buffer_size_cmd ;
    static QString get_connected_devices_cmd ;
    static QString select_active_device_cmd ;

    static QString get_active_channels_cmd ;
    static QString set_active_channels_cmd ;
    static QString reset_active_channels_cmd ;

    static QString set_performance_cmd ;
    static QString get_performance_cmd ;
    static QString is_processing_complete_cmd ;
    static QString is_analyzer_complete_cmd ;

    static QString set_digital_voltage_option_cmd ;
    static QString get_digital_voltage_options_cmd ;

    static QString close_all_tabs_cmd ;

    enum DeviceType
    {
        LOGIC_DEVICE,
        LOGIC_16_DEVICE,
        LOGIC_4_DEVICE,
        LOGIC_8_DEVICE,
        LOGIC_PRO_8_DEVICE,
        LOGIC_PRO_16_DEVICE
    };
    Q_ENUM(DeviceType)

    enum Trigger {
        None,
        HIGH,
        LOW,
        NEGEDGE,
        POSEDGE,
        NEGPULSE,
        POSPULSE
    };
    Q_ENUM(Trigger)

    enum PerformanceOption {
        OneHundredPercent = 100,
        EightyPercent = 80,
        SixtyPercent = 60,
        FortyPercent = 40,
        TwentyPercent = 20
    };
    Q_ENUM(PerformanceOption)

    //Export Data
    enum DataExportChannelSelection
    {
        ALL_CHANNELS,
        SPECIFIC_CHANNELS
    };
    Q_ENUM(DataExportChannelSelection)


    enum DataExportMixedModeExportType
    {
        DIGITAL_ONLY,
        ANALOG_ONLY,
        ANALOG_AND_DIGITAL
    };
    Q_ENUM(DataExportMixedModeExportType)

    enum DataExportSampleRangeType {
        ALL_TIME,
        TIME_SPAN
    };
    Q_ENUM(DataExportSampleRangeType)

    enum DataExportType {
        BINARY,
        CSV,
        VCD,
        MATLAB
    };
    Q_ENUM(DataExportType)

    enum CsvHeadersType
    {
        HEADERS,
        NO_HEADERS
    };
    Q_ENUM(CsvHeadersType)

    enum CsvDelimiterType
    {
        COMMA,
        TAB
    };
    Q_ENUM(CsvDelimiterType)

    enum CsvOutputMode
    {
        COMBINED,
        SEPARATE
    };
    Q_ENUM(CsvOutputMode)

    enum CsvTimestampType
    {
        TIME_STAMP,
        SAMPLE_NUMBER
    };
    Q_ENUM(CsvTimestampType)

    enum CsvBase
    {
        BIN,
        DEC,
        HEX,
        ASCII
    };
    Q_ENUM(CsvBase)

    enum CsvDensity
    {
        ROW_PER_CHANGE,
        ROW_PER_SAMPLE
    };
    Q_ENUM(CsvDensity)

    enum BinaryOutputMode
    {
        EACH_SAMPLE,
        ON_CHANGE
    };
    Q_ENUM(BinaryOutputMode)

    enum BinaryBitShifting
    {
        NO_SHIFT,
        RIGHT_SHIFT
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
        VOLTAGE,
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
            ANALOG,
            DIGITAL
        };

        int Index;// { get; set; }
        ChannelDataType DataType;// { get; set; }

        QString GetExportstring()
        {
            return  QString("%1 %2")
                    .arg(QString::number(Index), DataType == ANALOG ? "ANALOG" : "DIGITAL");
        }

    };

    struct DigitalVoltageOption
    {
        int Index;
        QString Description;
        bool IsSelected;
    };

    class ExportDataStruct
    {
    public:
        /// <summary>
        /// The fully qualified path to the target file. Folder must exist. Path must be absolute. Always required
        /// features like ~/ and %appdata% are not supported. You can exand those first before passing the path.
        /// </summary>
        QString FileName;

        /// <summary>
        /// This option is only required & applied IF your capture contains digital and analog channels, AND you select "Specific Channels" for the channel selection option.
        /// i. e. If you then select digital only, you will get the native, digital only interface for an export mode.
        /// </summary>
        DataExportMixedModeExportType DataExportMixedExportMode = ANALOG_AND_DIGITAL;

        /// <summary>
        /// This option allows you to export all channels in the capture, or only specific channels. Alaways required
        /// </summary>
        DataExportChannelSelection ExportChannelSelection = ALL_CHANNELS;
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
        DataExportSampleRangeType SamplesRangeType = ALL_TIME;
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
        DataExportType ExportType = CSV;


        /// <summary>
        /// Required for all CSV exports.
        /// </summary>
        CsvHeadersType CsvIncludeHeaders = HEADERS;
        /// <summary>
        /// Required for all CSV exports
        /// </summary>
        CsvDelimiterType CsvDelimiter = COMMA;
        /// <summary>
        /// Required only when exporting digital only CSV.
        /// Does not apply when exporting analog and digital or analog only.
        /// </summary>
        CsvOutputMode CsvOutput = COMBINED;
        /// <summary>
        /// Only applies when exporting digital only CSV
        /// Does not apply when exporting analog and digital or analog only. (time values used)
        /// </summary>
        CsvTimestampType CsvTimestamp = SAMPLE_NUMBER;
        /// <summary>
        /// Required when exporting analog samples as raw ADC value.
        /// Also Required when exporting digital only data in "CsvSingleNumber" format
        /// </summary>
        CsvBase CsvDisplayBase = DEC;
        /// <summary>
        /// Only required when exporting digital only CSV data.
        /// CsvTransition produces a smaller file where only transition timestamps are exported
        /// CsvComplete includes every single sample
        /// </summary>
        CsvDensity CsvDensityMode = ROW_PER_CHANGE;


        //Type: Binary
        /// <summary>
        /// Only required for digital only Binary mode
        /// </summary>
        BinaryOutputMode BinaryOutput = ON_CHANGE;
        /// <summary>
        /// Only required for digital only Binary mode
        /// </summary>
        BinaryBitShifting BinaryBitShiftingMode = NO_SHIFT;
        /// <summary>
        /// Only required for digital only Binary mode
        /// </summary>
        BinaryOutputWordSize BinOutputWordSize = Binary8Bit;

        /// <summary>
        /// ADC values or floating point voltages. Required for all export types that include analog channels
        /// </summary>
        AnalogOutputFormat AnalogFormat = VOLTAGE; //This feature needs v1.1.32+
    };

    QString CustomCommand( QString export_command );
    bool SetTrigger( QList<Trigger> triggers, double minimum_pulse_width_s = 0.0, double maximum_pulse_width_s = 1.0 );
    bool SetNumSamples( int num_samples );
    bool SetCaptureSeconds( double seconds );

    bool SetDigitalVoltageOption( DigitalVoltageOption option );
    QList<DigitalVoltageOption> GetDigitalVoltageOptions();

    bool CloseAllTabs();

    QList<SampleRate> GetAvailableSampleRates();
    bool SetSampleRate( SampleRate sample_rate );
    SampleRate GetSampleRate();

    bool CaptureToFile( QString file );
    bool SaveToFile( QString file );
    bool LoadFromFile( QString file );

    bool ExportData( ExportDataStruct export_data_struct );
    bool ExportData2( ExportDataStruct export_settings, bool capture_contains_digital_channels = true, bool capture_contains_analog_channels = true);

    QList<Analyzer> GetAnalyzers();
    bool ExportAnalyzers( int selected, QString filename, bool mXmitFile = false);

    bool Capture(int timeoutIn_ms = 0);
    bool StopCapture();

    int GetCapturePretriggerBufferSize();
    bool SetCapturePretriggerBufferSize( int buffer_size );

    QList<ConnectedDevice> GetConnectedDevices();

    bool SelectActiveDevice( int device_number );

    bool SetPerformanceOption( PerformanceOption performance );
    PerformanceOption GetPerformanceOption();

    bool IsProcessingComplete();
    bool IsAnalyzerProcessingComplete( int index );
    bool BlockUntillProcessingCompleteOrTimeout(const quint32 timeout );

    void GetActiveChannels( QList<int> digital_channels, QList<int> analog_channels );
    bool SetActiveChannels(QList<int> digital_channels, QList<int> analog_channels );

    bool ResetActiveChannels();

    bool isLogicConnected() const;
    void setLogicConnected(bool value);

private:
    void Writestring(const QString &str );

    bool GetResponse(int timeoutInMsec = 1000);
    QString GetResponseString(int timeoutInMsec = 100);

    bool TryParseDeviceType( QString input, DeviceType & device_type );

    const bool m_printCommandsToConsole = false;

    QTcpSocket *m_socket;
    int m_port;
    QString m_host;

    bool m_logicConnected;
};

}

#endif
