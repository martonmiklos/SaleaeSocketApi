# Saleae Socket API Qt Wrapper

The SaleaeSocketApi class provides the same API as the C# one to interact with the Saleae's Logic application. 

You can include the SaleaeSocketApi.pri file to your pro file, or you can build a shared library with the pro file and link your application to it. 

Many features untested, here is a list what we have been using so far:
* Starting, stopping the capture
* Exporting data mainly to digital formats with the ExportData2 method
* Opening log files

If you tested/fixed a specific feature feel free to submit a PR!  