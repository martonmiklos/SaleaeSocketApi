#include "SaleaeSocketApi.h"

#include <QMetaEnum>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QElapsedTimer>

namespace SaleaeSocketApi
{
SaleaeClient::SaleaeClient( QString host_str, int port_input, QObject *parent) :
    QObject(parent),
    m_port(port_input),
    m_host(host_str)
{
    m_socket = new QTcpSocket(this);
}

bool SaleaeClient::connectToLogic(QString host_str, int port_input)
{
    m_port = port_input;
    m_host = host_str;

    disconnectFromLogic();
    m_socket->setReadBufferSize(3);
    m_socket->connectToHost(host_str, port_input);
    if (m_socket->waitForConnected(1000)) {
        qWarning() << "Connected!";
        m_logicConnected = true;
        return true;
    } else {
        qWarning() << "Unable to connect to the Saleae Logic!";
        m_logicConnected = false;
        return false;
    }
}

void SaleaeClient::disconnectFromLogic()
{
    if (isLogicConnected()) {
        m_socket->disconnectFromHost();
        m_logicConnected = false;
    }
}

void SaleaeClient::Writestring(const QString &str )
{
    m_socket->readAll();
    m_socket->write(str.toLocal8Bit());
    m_socket->putChar('\0');
    m_socket->flush();
    qWarning() << "Wrote data: " << str;
}

bool SaleaeClient::GetResponse(int timeoutInMsec)
{
    QByteArray response;
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < timeoutInMsec) {
        m_socket->waitForReadyRead(1);
        if (m_socket->bytesAvailable()) {
            response.append(m_socket->readAll());         
        }

        if (response.startsWith("ACK")) {
            qWarning() << response;
            qWarning() << QString("Response time: %1 ms").arg(timer.elapsed());
            return true;
        } else if (response.startsWith("NAK")) {
            qWarning() << response;
            qWarning() << QString("Response time:: %1 ms").arg(timer.elapsed());
            return false;
        }
    }

    if(!response.isEmpty())
        qWarning() << response;

    qWarning() << QString("Error: Response timeout (%1 ms)!").arg(timeoutInMsec);
    return false;
}

QString SaleaeClient::GetResponseString(int timeoutInMsec)
{
    QString response;
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < timeoutInMsec) {
        m_socket->waitForReadyRead(1);
        response.append(m_socket->readAll());
    }
    qWarning() << response;
    return response;
}

bool SaleaeClient::TryParseDeviceType( QString input, DeviceType  & device_type )
{
    device_type = DeviceType::Logic; // default.

    /*var all_options = Enum.GetValues( typeof( DeviceType ) ).Cast<DeviceType>();
    FIXME
    if( all_options.Any( x => x.GetDescription() == input.Trim() ) )
    {
        device_type = all_options.Single( x => x.GetDescription() == input.Trim() );
        return true;
    }
    else
    {
        return false;
    }*/
    return false;
}

bool SaleaeClient::isLogicConnected() const
{
    if (m_logicConnected)
        return (m_socket->state() == QTcpSocket::ConnectedState);

    return false;
}

void SaleaeClient::setLogicConnected(bool value)
{
    m_logicConnected = value;
}

/// <summary>
/// Give the Socket API a custom command
/// </summary>
/// <param name="export_command">Ex: "set_sample_rate, 10000000"</param>
/// <returns>Response QString</returns>
QString SaleaeClient::CustomCommand( QString export_command )
{
    Writestring( export_command );

    QString response = "";
    while( response.isEmpty() )
    {
        response.append(m_socket->readAll());
    }

    return response;
}

/// <summary>
/// Set the capture trigger. Every active digital channel must be set, in order.
/// To ignore the maximum_pulse_width_s parameter, set it to 0.
/// </summary>
/// <param name="triggers">List of triggers for active channels. Ex"High, Low, Posedge, Negedge, Low, High, ..."</param>
bool SaleaeClient::SetTrigger( QList<Trigger> triggers, double minimum_pulse_width_s, double maximum_pulse_width_s )
{

    QStringList command;

    command.append ( set_trigger_cmd );

    /*if( triggers.Count( x => x == Trigger.PositivePulse || x == Trigger.NegativePulse || x == Trigger.RisingEdge || x == Trigger.FallingEdge ) > 1 )
        throw new SaleaeSocketApiException( "invalid trigger specifications" );
        FIXME
        */

    foreach (Trigger channel, triggers) {
        if (channel != None) {
            command.append(QMetaEnum::fromType<Trigger>().valueToKey(channel));
            if (channel == Trigger::POSPULSE || channel == Trigger::NEGPULSE) {
                command.append(QString::number(minimum_pulse_width_s));
                if( maximum_pulse_width_s > 0)
                    command.append(QString::number(maximum_pulse_width_s));
            }
        }
    }

    QString tx_command = command.join(",");
    Writestring( tx_command );

    return GetResponse();
}

/// <summary>
/// Set number of samples for capture
/// </summary>
/// <param name="num_samples">Number of samples to set</param>
bool SaleaeClient::SetNumSamples( int num_samples )
{
    QString export_command = set_num_samples_cmd + ", ";
    export_command += QString::number(num_samples);
    Writestring(export_command);

    return GetResponse();
}

/// <summary>
/// Set number of seconds to capture for
/// </summary>
/// <param name="capture_seconds">Number of seconds to capture</param>
bool SaleaeClient::SetCaptureSeconds( double seconds )
{
    QString export_command = set_capture_seconds_cmd + ", ";
    export_command += QString::number(seconds);
    Writestring( export_command );

    return GetResponse();
}

bool SaleaeClient::SetDigitalVoltageOption(DigitalVoltageOption option)
{
    QString command = set_digital_voltage_option_cmd;
    command += ", " + QString::number(option.Index);
    Writestring( command );
    return GetResponse();
}

/// <summary>
/// Requires 1.2.6 or later software, otherwise it will throw
/// </summary>
/// <returns>List of possible IO threshold settings, if the active device supports multiple threshold voltages</returns>
QList<SaleaeClient::DigitalVoltageOption> SaleaeClient::GetDigitalVoltageOptions()
{
    QString command = get_digital_voltage_options_cmd;
    Writestring( command );

    QString response = GetResponseString();
    //var elems = response.split(',', '\n' ).Select( x => x.Trim() ).ToList();
    QStringList elems;
    foreach (QString row, response.split('\n')) {
        foreach (QString rowItem, row.split(',')) {
            elems.append(rowItem.trimmed());
        }
    }

    int voltage_count = elems.count() / 3;
    QList<DigitalVoltageOption> voltage_options;

    for (int i = 0; i < voltage_count; i++) {
        int index = i * 3;
        DigitalVoltageOption option = {
            elems[index].toInt(),                                                                       // Index
            elems[index+1],                                                                             // Description
            elems[ index + 2 ].contains( "SELECTED" ) && !elems[ index + 2 ].contains( "NOT_SELECTED" ) // IsSelected
        };
        voltage_options.append(option);
    }
    return voltage_options;

}

/// <summary>
/// Closes all currently open tabs.
/// </summary>
bool SaleaeClient::CloseAllTabs()
{
    QString export_command = close_all_tabs_cmd;
    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// Set the sample rate for capture
/// </summary>
/// <param name="sample_rate">Sample rate to set</param>
bool SaleaeClient::SetSampleRate( SampleRate sample_rate )
{
    QString export_command = set_sample_rate_cmd + ", ";
    export_command += QString::number(sample_rate.DigitalSampleRate);
    export_command += ", " + QString::number(sample_rate.AnalogSampleRate);

    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// Return the currently selected sample rate.
/// </summary>
/// <returns>Currently Selected Sample Rate</returns>
SampleRate SaleaeClient::GetSampleRate()
{
    QString command = get_sample_rate_cmd;
    Writestring( command );

    QString response = GetResponseString();

    //var elems = response.split( '\n' ).Take(2).Select( x => int.Parse( x.Trim() ) ).ToList();
    QList<int> elems;
    foreach (QString elem, response.split('\n')) {
        elems.append(elem.toInt());
    }

    if( elems.count() != 2 )
        throw new SaleaeSocketApiException( "unexpected value" );

    return SampleRate(elems[0], elems[1]);

}

/// <summary>
/// Start capture and save when capture finishes
/// </summary>
/// <param name="file">File to save capture to</param>
bool SaleaeClient::CaptureToFile( QString file )
{
    QString export_command = capture_to_file_cmd + ", ";
    export_command += file;
    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// Save active tab capture to file
/// </summary>
/// <param name="file">File to save capture to</param>
bool SaleaeClient::SaveToFile( QString file )
{
    QString export_command = save_to_file_cmd + ", ";
    export_command += file;
    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// Load a saved capture from fil
/// </summary>
/// <param name="file">File to load</param>
bool SaleaeClient::LoadFromFile( QString file )
{
    QString export_command = load_from_file_cmd + ", ";
    export_command += file;
    Writestring( export_command );

    return GetResponse(3000);
}

//create input struct
bool SaleaeClient::ExportData( ExportDataStruct export_data_struct )
{
    //channels
    const QString all_channels_option = ", ALL_CHANNELS";
    const QString digital_channels_option = ", DIGITAL_CHANNELS";
    const QString analog_channels_option = ", ANALOG_CHANNELS";

    //time span
    const QString all_time_option = ", ALL_TIME";
    const QString time_span_option = ", TIME_SPAN";

    const QString csv_option = ", CSV";
    const QString headers_option = ", HEADERS";
    const QString no_headers_option = ", NO_HEADERS";
    const QString tab_option = ", TAB";
    const QString comma_option = ", COMMA";
    const QString sample_number_option = ", SAMPLE_NUMBER";
    const QString time_stamp_option = ", TIME_STAMP";
    const QString combined_option = ", COMBINED";
    const QString separate_option = ", SEPARATE";
    const QString row_per_change_option = ", ROW_PER_CHANGE";
    const QString row_per_sample_option = ", ROW_PER_SAMPLE";
    const QString dec_option = ", DEC";
    const QString hex_option = ", HEX";
    const QString bin_option = ", BIN";
    const QString ascii_option = ", ASCII";

    const QString binary_option = ", BINARY";
    const QString each_sample_option = ", EACH_SAMPLE";
    const QString on_change_option = ", ON_CHANGE";

    const QString voltage_option = ", VOLTAGE";
    const QString raw_adc_option = ", ADC";
    const QString vcd_option = ", VCD";
    const QString matlab_option = ", MATLAB";


    QString export_command = export_data_cmd;
    export_command += ", " + export_data_struct.FileName;

    if( export_data_struct.ExportChannelSelection == DataExportChannelSelection::ALL_CHANNELS )
        export_command += all_channels_option;
    else
    {
        if( export_data_struct.DigitalChannelsToExport.length() > 0 )
        {
            export_command += digital_channels_option;
            foreach( int channel,  export_data_struct.DigitalChannelsToExport )
                export_command += ", " + QString::number(channel);
        }

        if( export_data_struct.AnalogChannelsToExport.length() > 0 )
        {
            export_command += analog_channels_option;
            foreach( int channel, export_data_struct.AnalogChannelsToExport )
                export_command += ", " + QString::number(channel);
        }
    }

    if (
            (export_data_struct.ExportChannelSelection == DataExportChannelSelection::ALL_CHANNELS) ||
            (export_data_struct.AnalogChannelsToExport.length() > 0)
            )
    {
        if( export_data_struct.AnalogFormat == AnalogOutputFormat::VOLTAGE )
            export_command += voltage_option;
        else if( export_data_struct.AnalogFormat == AnalogOutputFormat::ADC )
            export_command += raw_adc_option;
    }

    if( export_data_struct.SamplesRangeType == DataExportSampleRangeType::ALL_TIME )
        export_command += all_time_option;
    else if( export_data_struct.SamplesRangeType == DataExportSampleRangeType::TIME_SPAN )
    {
        export_command += time_span_option;
        export_command += ", " + QString::number(export_data_struct.StartingTime);
        export_command += ", " + QString::number(export_data_struct.EndingTime);
    }

    switch (export_data_struct.ExportType) {
    case DataExportType::CSV:
        export_command += csv_option;

        if( export_data_struct.CsvIncludeHeaders == CsvHeadersType::HEADERS )
            export_command += headers_option;
        else if( export_data_struct.CsvIncludeHeaders == CsvHeadersType::NO_HEADERS )
            export_command += no_headers_option;

        if( export_data_struct.CsvDelimiter == CsvDelimiterType::TAB )
            export_command += tab_option;
        else if( export_data_struct.CsvDelimiter == CsvDelimiterType::COMMA )
            export_command += comma_option;

        if( export_data_struct.CsvTimestamp == CsvTimestampType::SAMPLE_NUMBER )
            export_command += sample_number_option;
        else if( export_data_struct.CsvTimestamp == CsvTimestampType::TIME_STAMP )
            export_command += time_stamp_option;

        if( export_data_struct.CsvOutput == CsvOutputMode::COMBINED )
            export_command += combined_option;
        else if( export_data_struct.CsvOutput == CsvOutputMode::SEPARATE )
            export_command += separate_option;

        if( export_data_struct.CsvDensityMode == CsvDensity::ROW_PER_CHANGE )
            export_command += row_per_change_option;
        else if( export_data_struct.CsvDensityMode == CsvDensity::ROW_PER_SAMPLE )
            export_command += row_per_sample_option;

        if( export_data_struct.CsvDisplayBase == CsvBase::DEC )
            export_command += dec_option;
        else if( export_data_struct.CsvDisplayBase == CsvBase::HEX )
            export_command += hex_option;
        else if( export_data_struct.CsvDisplayBase == CsvBase::BIN )
            export_command += bin_option;
        else if( export_data_struct.CsvDisplayBase == CsvBase::ASCII )
            export_command += ascii_option;
        break;
    case DataExportType::BINARY:
        export_command += binary_option;

        if( export_data_struct.BinaryOutput == BinaryOutputMode::EACH_SAMPLE )
            export_command += each_sample_option;
        else if( export_data_struct.BinaryOutput == BinaryOutputMode::ON_CHANGE )
            export_command += on_change_option;

        export_command.append(", ");
        export_command.append(QMetaEnum::fromType<BinaryOutputWordSize>().valueToKey(export_data_struct.BinOutputWordSize));
        break;
    case DataExportType::VCD:
        export_command += vcd_option;
        break;
    case DataExportType::MATLAB:
        export_command += matlab_option;
        break;
    }

    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// This replaced the hard to use and buggy EXPORT_DATA command.
/// </summary>
/// <param name="export_settings"></param>
/// <param name="capture_contains_digital_channels"></param>
/// <param name="capture_contains_analog_channels"></param>
/// <returns></returns>
///
/// TODO: make bool arguments enums!!!
bool SaleaeClient::ExportData2( ExportDataStruct export_settings, bool export_digital_channels, bool export_analog_channels )
{
    bool is_mixed_mode_capture = export_digital_channels && export_analog_channels; //different export options happen in this case.

    if (export_analog_channels && !export_digital_channels)
        export_settings.DataExportMixedExportMode = ANALOG_ONLY;

    if (export_digital_channels && !export_analog_channels)
        export_settings.DataExportMixedExportMode = DIGITAL_ONLY;

    if( is_mixed_mode_capture && export_settings.ExportChannelSelection == DataExportChannelSelection::ALL_CHANNELS)
        export_settings.DataExportMixedExportMode = DataExportMixedModeExportType::ANALOG_AND_DIGITAL; //this is not required to be explicitly set by the user.

    QStringList command_parts;
    command_parts.append( export_data2_cmd );

    command_parts.append( export_settings.FileName );

    command_parts.append( QMetaEnum::fromType<DataExportChannelSelection>().valueToKey(export_settings.ExportChannelSelection) );

    if (export_settings.ExportChannelSelection == DataExportChannelSelection::SPECIFIC_CHANNELS) {
        command_parts.append( QMetaEnum::fromType<DataExportMixedModeExportType>().valueToKey(export_settings.DataExportMixedExportMode) );

        foreach (int channel, export_settings.DigitalChannelsToExport) {
            Channel ch;
            ch.Index = channel;
            ch.DataType = Channel::DIGITAL;
            command_parts.append(ch.GetExportstring());
        }

        foreach (int channel, export_settings.AnalogChannelsToExport) {
            Channel ch;
            ch.Index = channel;
            ch.DataType = Channel::ANALOG;
            command_parts.append(ch.GetExportstring());
        }
    }

    //time options.
    command_parts.append(QMetaEnum::fromType<DataExportSampleRangeType>().valueToKey(export_settings.SamplesRangeType));

    if (export_settings.SamplesRangeType == DataExportSampleRangeType::TIME_SPAN) {
        command_parts.append( QString::number(export_settings.StartingTime) );
        command_parts.append( QString::number(export_settings.EndingTime) );
    }

    command_parts.append( QMetaEnum::fromType<DataExportType>().valueToKey(export_settings.ExportType) );
    //digital only CSV
    if (export_digital_channels && export_settings.ExportType == DataExportType::CSV &&
            (!is_mixed_mode_capture || export_settings.DataExportMixedExportMode == DataExportMixedModeExportType::DIGITAL_ONLY)) {
        command_parts.append(QMetaEnum::fromType<CsvHeadersType>().valueToKey(export_settings.CsvIncludeHeaders));
        command_parts.append(QMetaEnum::fromType<CsvDelimiterType>().valueToKey(export_settings.CsvDelimiter));
        command_parts.append(QMetaEnum::fromType<CsvTimestampType>().valueToKey(export_settings.CsvTimestamp));
        command_parts.append(QMetaEnum::fromType<CsvOutputMode>().valueToKey(export_settings.CsvOutput));
        if( export_settings.CsvOutput == CsvOutputMode::COMBINED )
            command_parts.append(QMetaEnum::fromType<CsvBase>().valueToKey(export_settings.CsvDisplayBase));
        command_parts.append(QMetaEnum::fromType<CsvDensity>().valueToKey(export_settings.CsvDensityMode));
    }

    //analog only CSV
    if( export_analog_channels && export_settings.ExportType == DataExportType::CSV &&
            (!is_mixed_mode_capture || export_settings.DataExportMixedExportMode == DataExportMixedModeExportType::ANALOG_ONLY ) ) {
        command_parts.append( QMetaEnum::fromType<CsvHeadersType>().valueToKey(export_settings.CsvIncludeHeaders) );
        command_parts.append( QMetaEnum::fromType<CsvDelimiterType>().valueToKey(export_settings.CsvDelimiter) );
        command_parts.append( QMetaEnum::fromType<CsvBase>().valueToKey(export_settings.CsvDisplayBase) );
        command_parts.append( QMetaEnum::fromType<AnalogOutputFormat>().valueToKey(export_settings.AnalogFormat) );

    }

    //mixed mode CSV
    if (export_settings.ExportType == DataExportType::CSV &&
            is_mixed_mode_capture &&
            export_settings.DataExportMixedExportMode == DataExportMixedModeExportType::ANALOG_AND_DIGITAL ) {
        command_parts.append( QMetaEnum::fromType<CsvHeadersType>().valueToKey(export_settings.CsvIncludeHeaders) );
        command_parts.append( QMetaEnum::fromType<CsvDelimiterType>().valueToKey(export_settings.CsvDelimiter) );
        command_parts.append( QMetaEnum::fromType<CsvBase>().valueToKey(export_settings.CsvDisplayBase) );
        command_parts.append( QMetaEnum::fromType<AnalogOutputFormat>().valueToKey(export_settings.AnalogFormat) );
    }

    //digital binary
    if (export_digital_channels &&  export_settings.ExportType == DataExportType::BINARY &&
            ( !is_mixed_mode_capture || export_settings.DataExportMixedExportMode == DataExportMixedModeExportType::DIGITAL_ONLY ) ) {
        command_parts.append( QMetaEnum::fromType<BinaryOutputMode>().valueToKey(export_settings.BinaryOutput) );
        command_parts.append( QMetaEnum::fromType<BinaryBitShifting>().valueToKey(export_settings.BinaryBitShiftingMode) );
        command_parts.append( QString::number(export_settings.BinOutputWordSize) );
    }

    //analog only binary
    if (export_analog_channels &&
            export_settings.ExportType == DataExportType::BINARY &&
            ( is_mixed_mode_capture || export_settings.DataExportMixedExportMode == DataExportMixedModeExportType::ANALOG_ONLY ) ) {
        command_parts.append( QMetaEnum::fromType<AnalogOutputFormat>().valueToKey(export_settings.AnalogFormat) );
    }

    //VCD (always digital only)
    //if( export_settings.ExportType == DataExportType::VCD ) {
    //no settings
    //}

    //Matlab digital:
    //if( capture_contains_digital_channels && export_settings.ExportType == DataExportType::MATLAB && ( !is_mixed_mode_capture || export_settings.DataExportMixedExportMode == DataExportMixedModeExportType::DIGITAL_ONLY ) ){
    //no settings
    //}

    //Matlab analog or mixed:
    if( export_analog_channels &&
            export_settings.ExportType == DataExportType::MATLAB &&
            (!is_mixed_mode_capture || export_settings.DataExportMixedExportMode != DataExportMixedModeExportType::DIGITAL_ONLY ) ) {
        command_parts.append( QMetaEnum::fromType<AnalogOutputFormat>().valueToKey(export_settings.AnalogFormat) );
    }


    QString socket_command = command_parts.join(", ");
    Writestring( socket_command );


    return GetResponse(5000);
}

/// <summary>
/// Get the active analyzers on the current tab
/// </summary>
/// <returns>A QString of the names of the analyzers</returns>
QList<Analyzer> SaleaeClient::GetAnalyzers()
{
    QString export_command = get_analyzers_cmd;
    Writestring( export_command );

    QString response = GetResponseString();

    //QStringList lines = response.split( '\n' ).Select( x => x.Trim() ).Where( x => !QString.IsNullOrWhiteSpace( x ) && !x.Contains( "ACK" ) ).ToList();
    QStringList lines;
    foreach (QString line, response.split( '\n' )) {
        if (!line.contains("ACK") && !line.trimmed().isEmpty()) {
            lines.append(line.trimmed());
        }
    }

    QList<Analyzer> analyzers;

    foreach(QString line, lines)
    {
        //var elements = line.split( ',' ).Select( x => x.Trim() ).ToList();
        QStringList elements = line.split(',');
        Analyzer analyzer;
        analyzer.AnalyzerType = elements.at(0).toInt();
        analyzer.Index = elements.at(0).toInt();
        analyzers.append(analyzer);
    }

    return analyzers;
}

/// <summary>
/// Export a selected analyzer to a file
/// </summary>
/// <param name="selected">index of the selected analyzer(GetAnalyzer return QString index + 1)</param>
/// <param name="filename">file to save analyzer to</param>
/// <param name="mXmitFile">mXmitFile</param>
bool SaleaeClient::ExportAnalyzers( int selected, QString filename, bool mXmitFile )
{
    QString export_command = export_analyzer_cmd + ", ";
    export_command += QString::number(selected) + ", " + filename;
    if( mXmitFile == true )
        export_command += ", mXmitFile";
    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// Start device capture
/// </summary>
bool SaleaeClient::Capture()
{
    Writestring( capture_cmd );
    return GetResponse(100);
}

/// <summary>
/// Stop the current capture
/// </summary>

bool SaleaeClient::StopCapture()
{
    QString export_command = stop_capture_cmd;
    Writestring( export_command );

    return GetResponse();
}


/// <summary>
/// Get size of pre-trigger buffer
/// </summary>
/// <returns>buffer size in # of samples</returns>
int SaleaeClient::GetCapturePretriggerBufferSize()
{
    QString export_command = get_capture_pretrigger_buffer_size_cmd;
    Writestring( export_command );

    QString response = GetResponseString();
    QStringList input_string = response.split( '\n' );
    int buffer_size = input_string.at(0).toInt();
    return buffer_size;
}

/// <summary>
/// set pre-trigger buffer size
/// </summary>
/// <param name="buffer_size">buffer size in # of samples</param>
bool SaleaeClient::SetCapturePretriggerBufferSize( int buffer_size )
{
    QString export_command = set_capture_pretrigger_buffer_size_cmd + ", ";
    export_command += QString::number(buffer_size);
    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// Return the devices connected to the software
/// </summary>
/// <returns>Array of ConnectedDevices structs which contain device information</returns>
QList<SaleaeClient::ConnectedDevice> SaleaeClient::GetConnectedDevices()
{
    QString command = get_connected_devices_cmd;
    Writestring( command );

    QString response = GetResponseString();
    //var response_strings = response.split( '\n' ).ToList();
    QStringList response_strings = response.split('\n');
    for (int i = 0; i<response_strings.count(); i++) {
        if (response_strings.at(i).contains("ACK"))
            response_strings.removeAt(i);
    }

    QList<ConnectedDevice> devices;
    foreach( QString line , response_strings)
    {
        //var elements = line.split( ',' ).Select( x => x.Trim() ).ToList();
        QStringList elements = line.split(',');

        DeviceType device_type;

        if( TryParseDeviceType( elements[ 2 ], device_type ) == false )
            throw new SaleaeSocketApiException( "unexpected value" );

        ConnectedDevice device = {
            device_type, // DeviceType
            elements[ 1 ], // Name
            (quint64)elements[ 3 ].toLongLong(NULL, 16), // DeviceId
            elements[ 0 ].toInt(), // Index
            (elements.count() == 5 && elements[4] == "ACTIVE") ? true : false // IsActive
        };
        devices.append( device );
    }

    return devices;
}

/// <summary>
/// Select the active capture device
/// </summary>
/// <param name="device_number">Index of device (as returned from ConnectedDevices struct)</param>
bool SaleaeClient::SelectActiveDevice( int device_number )
{
    QString export_command = select_active_device_cmd + ", ";
    export_command += QString::number(device_number);
    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// Set the performance option
/// </summary>
bool SaleaeClient::SetPerformanceOption( PerformanceOption performance )
{
    QString export_command = set_performance_cmd + ", ";
    export_command += QString::number(performance);
    Writestring( export_command );
    return GetResponse();
}

/// <summary>
/// Get the performance option currently selected.
/// </summary>
/// <returns>A PerformanceOption enum</returns>
SaleaeClient::PerformanceOption SaleaeClient::GetPerformanceOption()
{
    QString export_command = get_performance_cmd;
    Writestring( export_command );

    QString response = GetResponseString();

    PerformanceOption selected_option = ( PerformanceOption )response.split( '\n' ).first().toInt();
    return selected_option;
}


/// <summary>
/// Get whether or not the software is done processing data. You must wait for data to be finished processing before you can export/save. 
/// </summary>
/// <returns>A boolean indicating if processing is complete</returns>

bool SaleaeClient::IsProcessingComplete()
{
    QString export_command = is_processing_complete_cmd;
    Writestring( export_command );

    QString response = GetResponseString();

    bool complete_processing = response.split( '\n' ).first().toInt();
    return complete_processing;
}

/// <summary>
/// Get whether or not the software is done processing data. You must wait for data to be finished processing before you can export/save. 
/// </summary>
/// <returns>A boolean indicating if processing is complete</returns>

bool SaleaeClient::IsAnalyzerProcessingComplete( int index )
{
    QString export_command = is_analyzer_complete_cmd;
    export_command += ", " + QString::number( index );
    Writestring( export_command );

    QString response = GetResponseString();

    bool complete_processing = response.split( '\n' ).first().toInt();
    return complete_processing;
}

/// <summary>
/// Calls IsProcessingComplete every 250 ms.
/// </summary>
/// <param name="timeout"></param>
/// <returns></returns>
bool SaleaeClient::BlockUntillProcessingCompleteOrTimeout(const quint32 timeoutInMs )
{
    quint64 processing_timeout = 0;
    bool processing_finished = false;
    do
    {
        processing_finished = IsProcessingComplete();

        if( !processing_finished ) {
            QThread::msleep(250);
            processing_timeout += 250;
        }
    }
    while( !processing_finished && processing_timeout < timeoutInMs );

    return processing_finished;
}


/// <summary>
/// Get the currently available sample rates for the selected performance options
/// </summary>
/// <returns>Array of sample rate combinations available</returns>
QList<SampleRate> SaleaeClient::GetAvailableSampleRates()
{
    Writestring( get_all_sample_rates_cmd );
    QString response = GetResponseString();

    QList<SampleRate> sample_rates;
    QStringList responses = response.split("\n", QString::SkipEmptyParts);

    for( int i = 0; i < responses.length() - 1; i++ ) {
        QStringList split_sample_rate = responses[ i ].split( ',' );
        if( split_sample_rate.length() != 2 )
        {
            sample_rates.clear();
            return sample_rates;
        }

        SampleRate new_sample_rate(split_sample_rate[ 0 ].toInt(), split_sample_rate[ 1 ].toInt());
        sample_rates.append( new_sample_rate );
    }

    return sample_rates;
}

/// <summary>
/// Get active channels for devices Logic16, Logic 8(second gen), Logic 8 pro, Logic 16 pro
/// </summary>
/// <returns>array of active channel numbers</returns>
void SaleaeClient::GetActiveChannels( QList<int> digital_channels, QList<int> analog_channels )
{
    QString export_command = get_active_channels_cmd;
    Writestring( export_command );

    QString response = GetResponseString();

    digital_channels.clear();
    analog_channels.clear();

    QStringList input_string = response.split( '\n' );
    QStringList channels_string = input_string[ 0 ].split( ',' );

    bool add_to_digital_channel_list = true;
    for( int i = 0; i < channels_string.length(); ++i )
    {
        if( channels_string[ i ] == "digital_channels" )
        {
            add_to_digital_channel_list = true;
            continue;
        }
        else if( channels_string[ i ] == "analog_channels" )
        {
            add_to_digital_channel_list = false;
            continue;
        }

        if( add_to_digital_channel_list )
            digital_channels.append(channels_string[ i ].toInt());
        else
            analog_channels.append(channels_string[ i ].toInt());
    }

}

/// <summary>
/// Set the active channels for devices Logic16, Logic 8(second gen), Logic 8 pro, Logic 16 pro
/// </summary>
/// <param name="channels">array of channels to be active: 0-15</param>
bool SaleaeClient::SetActiveChannels( QList <int> digital_channels, QList<int >analog_channels)
{

    QString export_command = set_active_channels_cmd;
    if( digital_channels.count() )
    {
        export_command.append(", digital_channels");
        for( int i = 0; i < digital_channels.length(); ++i )
            export_command.append(", " + QString::number(digital_channels[ i ]));
    }
    if( analog_channels.count() )
    {
        export_command.append(", analog_channels");
        for( int i = 0; i < analog_channels.length(); ++i )
            export_command.append(", " + QString::number(analog_channels[ i ]));
    }
    Writestring( export_command );

    return GetResponse();
}

/// <summary>
/// Reset to default active logic 16 channels (0-15)
/// </summary>
bool SaleaeClient::ResetActiveChannels()
{
    QString export_command = reset_active_channels_cmd;
    Writestring( export_command );

    return GetResponse();
}

} // end namespace SaleaeSocketApi
