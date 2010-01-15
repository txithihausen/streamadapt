/*
 * plugin_stream.h
 *
 *  Created on: 19/12/2009
 *      Author: ivocalado
 */

#ifndef PLUGIN_STREAM_H_
#define PLUGIN_STREAM_H_
#include "pluginbase.h"
#include <map>
#include <string>
#include <cc++/config.h>

class PluginStreamIF: public PluginBase {
protected:
	virtual void adaptStream(std::string param, std::map<std::string,
			std::string> &params) throw (OperationNotPerfomedException,
			OperationNotSupportedException) = 0; // Adicionar metodos padrao para adaptacao a nivel de stream
public:

	enum StreamType {
		ENCODER_SIDE, DECODER_SIDE, BOTH_SIDES
	};

	virtual void adapt(std::string param,
			std::map<std::string, std::string> &params)
			throw (OperationNotPerfomedException,
			OperationNotPerfomedException) {
		adaptStream(param, params);
	}

	// Encode a 16-bit PCM sample buffer to a encoded payload
	// Returns the number of bytes written into the payload.
	// The silence flag indicates if the returned sound samples represent silence
	// that may be suppressed.
	virtual uint16 encode(int16 *sample_buf, uint16 nsamples, uint8 *payload,
			uint16 payload_size, bool &silence) throw(OperationNotPerfomedException) = 0;

	virtual uint16 decode(uint8 *payload, uint16 payload_size, int16 *pcm_buf,
			uint16 pcm_buf_size) throw(OperationNotPerfomedException) = 0;

	virtual const char* getName() const = 0;

};

#endif /* PLUGIN_STREAM_H_ */
