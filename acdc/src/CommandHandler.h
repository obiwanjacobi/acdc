#pragma once
#include <stdint.h>

template <class TextWriterT>
class CommandHandler
{
public:
    void SetTextWriter(TextWriterT *writer)
    {
        _writer = writer;
    }

    void OnPower(bool on, uint8_t track)
    {
        if (_writer)
            _writer->WriteLine("OnPower");

        // do something
    }

private:
    TextWriterT *_writer;
};
