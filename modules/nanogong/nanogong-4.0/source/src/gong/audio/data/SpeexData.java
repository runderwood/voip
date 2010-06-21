/*
 * Copyright 2002-2010 The Gong Project (http://gong.ust.hk)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package gong.audio.data;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import javax.sound.sampled.AudioFormat;
import org.xiph.speex.SpeexEncoder;
import org.xiph.speex.SpeexDecoder;

/**
 * This class stores audio data using the speex codec.
 * @author Gibson Lam
 * @version 3.0, 12/08/2008
 * @version 4.0, 18/01/2010
 */
public class SpeexData extends BlockAudioData {
    
    /**
     * The extension used by Speex files
     */
    public static final String FILE_EXTENSION = ".spx";
    
    /**
     * The default speex quality
     */
    public static final int SPEEX_DEFAULT_QUALITY = 5;
    
    /** The frames per packet in the Ogg page */
    private int framesPerPacket;
    
    /**
     * The speex encoder for the blocks
     */
    private SpeexEncoder encoder = new SpeexEncoder();
    
    /** The Ogg writer for the Speex data */
    private OggSpeexWriter writer;
    
    /** The speex decoder for the blocks */
    private SpeexDecoder decoder = new SpeexDecoder();
    
    /** The decoded block data */
    private Hashtable decodedData = new Hashtable();
    
    /**
     * The number of bytes written to the cache
     */
    protected long cacheSize = 0;
    
    /**
     * Creates a new instance of SpeexData
     */
    public SpeexData() {
        super();
    }
    
    /**
     * Creates a new instance of SpeexData
     * @param format the audio format
     * @param vbr encoding with variable bit rate
     * @param quality encoding quality
     */
    public SpeexData(AudioFormat format, boolean vbr, int quality) {
        super(format);
        
        int mode = 0;
        if (format.getSampleRate() <= 8000)
            mode = 0;
        else if (format.getSampleRate() <= 16000)
            mode = 1;
        else
            mode = 2;
        decoder.init(mode, (int) format.getSampleRate(), format.getChannels(), true);
        
        encoder.init(mode, quality, (int) format.getSampleRate(), format.getChannels());
        encoder.getEncoder().setVbr(vbr);
        encoder.getEncoder().setVbrQuality((float) quality);
        framesPerPacket = 1;
        samplesPerBlock = encoder.getFrameSize();
        
        writer = new OggSpeexWriter(mode, (int) format.getSampleRate(), format.getChannels(), 1, vbr);
    }
    
    /**
     * Creates a new instance of SpeexData
     * @param format the audio format
     */
    public SpeexData(AudioFormat format) {
        this(format, true, SPEEX_DEFAULT_QUALITY);
    }
    
    /**
     * Returns the file extension of the data
     * @return the file extension
     */
    public String getFileExtension() {
        return FILE_EXTENSION;
    }
    
    /**
     * Creates a new block
     * @return the new block
     */
    protected Block createBlock() {
        return new SpeexBlock(samplesPerBlock);
    }
    
    /**
     * Closes the audio data
     * @throws java.lang.Exception failed to close the audio data
     */
    public void close() throws Exception {
        for (Enumeration en = blockData.elements(); en.hasMoreElements();) {
            SpeexBlock block = (SpeexBlock) en.nextElement();
            if (block.getData() == null) block.encodeData();
        }
    }
    
    /**
     * Gets the memory usage the audio data
     * @return the length of memory usage
     */
    public synchronized long getMemoryUsage() {
        long size = 0;
        for (Enumeration en = blockData.elements(); en.hasMoreElements();) {
            SpeexBlock block = (SpeexBlock) en.nextElement();
            if (block.getData() != null) size += block.getData().length;
        }
        return size;
    }
    
    /**
     * Creates a clone of this class
     * @return the cloned audio data
     */
    public synchronized Object clone() {
        SpeexData audioData = new SpeexData(format);
        for (Enumeration en = blockData.elements(); en.hasMoreElements();) {
            audioData.blockData.add(((SpeexBlock) en.nextElement()).clone());
        }
        audioData.framesPerPacket = framesPerPacket;
        audioData.samplesPerBlock = samplesPerBlock;
        audioData.availableBlocks = availableBlocks;
        audioData.decoder = decoder;
        audioData.encoder = encoder;
        
        return audioData;
    }
    
    /** Sends the speex header to the Ogg file
     * @param file the Ogg file
     * @throw Failed to send header to the Ogg file
     */
    private void sendHeaderToOggWriter(OggSpeexWriter writer) throws Exception {
        writer.writeHeader("Stream encoded in Gong");
    }
    
    /** Sends the block data to the Ogg file
     * @param file the Ogg file
     * @throw Failed to send data to the Ogg file
     */
    private void sendDataToOggWriter(OggSpeexWriter writer) throws Exception {
        for (Enumeration en = blockData.elements(); en.hasMoreElements();) {
            SpeexBlock block = (SpeexBlock) en.nextElement();
            block.sendToOggWriter(writer);
        }
    }
    
    /**
     * Sends the audio data to the output stream
     * @param stream the output stream
     * @throws java.lang.Exception failed to send data to stream
     */
    public synchronized void sendToStream(OutputStream stream) throws Exception {
        writer.open("");
            
        sendHeaderToOggWriter(writer);
        sendDataToOggWriter(writer);
            
        writer.close();
        byte[] buffer = writer.getData();
        stream.write(writer.getData());
    }
    
    /**
     * Receives the Ogg header from the input stream
     * @param stream the input stream
     * @throws java.lang.Exception failed to receive Ogg header from stream
     * @return the Ogg header
     */
    protected OggHeader receiveOggHeaderFromStream(InputStream stream) throws Exception {
        DataInputStream dataStream = new DataInputStream(stream);
        byte[] buffer = new byte[4];
        OggHeader header = new OggHeader();
        
        receiveByteArrayFromStream(dataStream, buffer, 0, 4);       // Ogg Id
        if (!(new String(buffer, 0, 4).equals("OggS"))) throw new Exception("Invalid Ogg id ('OggS').");
        dataStream.read();                                          // Capture Pattern
        
        header.headerType = dataStream.read();                      // Header Type
        header.granulePos = swapLong(dataStream.readLong());        // Granule Position
        header.serial = swapInt(dataStream.readInt());              // Serial Number
        header.sequence = swapInt(dataStream.readInt());            // Page Sequence Number
        header.checksum = swapInt(dataStream.readInt());            // Page Checksum
        header.segments = dataStream.read();                        // Page Segments
        header.segmentSize = new byte[header.segments];             // Segment Size
        receiveByteArrayFromStream(dataStream, header.segmentSize, 0, header.segments);

        return header;
    }
    
    /** Checks the speex mode
     * @param rate the sampling rate
     * @param mode the speex mode
     * @return whether the mode is correct
     */
    private boolean checkSpeexMode(int rate, int mode) {
        if (rate <= 8000) {
            return (mode == 0);
        }
        if (rate <= 16000) {
            return (mode == 1);
        }
        return (mode == 2);
    }
    
    /** Receives the Speex header from the input stream
     * @param stream the input stream
     * @throw Failed to receive Speex header from stream
     */
    private void receiveSpeexHeaderFromStream(InputStream stream) throws Exception {
        DataInputStream dataStream = new DataInputStream(stream);
        byte[] buffer = new byte[20];
        
        receiveByteArrayFromStream(dataStream, buffer, 0, 8);               // Speex String
        if (!(new String(buffer, 0, 8).equals("Speex   "))) throw new Exception("Invalid Speex string ('Speex   ').");
        receiveByteArrayFromStream(dataStream, buffer, 0, 20);              // Speex Version
        if (!(new String(buffer, 0, 9).equals("speex-1.0"))) throw new Exception("Invalid Speex string ('speex-1.0').");
        int versionId = swapInt(dataStream.readInt());              // Speex Version Id
        if (versionId != 1) throw new Exception("Invalid Speex version id.");
        int headerSize = swapInt(dataStream.readInt());             // Header Size
        if (headerSize != 80) throw new Exception("Invalid Header size.");
        int sampleRate = swapInt(dataStream.readInt());             // Sample Rate
        format = new AudioFormat(sampleRate, 16, 1, true, true);
        int mode = swapInt(dataStream.readInt());                   // Mode
        if (!checkSpeexMode(sampleRate, mode)) throw new Exception("Invalid Speex mode.");
        int bitstreamVersion = swapInt(dataStream.readInt());       // Bitstream Version
        if (bitstreamVersion != 4) throw new Exception("Invalid Bitstream version.");
        int channels = swapInt(dataStream.readInt());               // Channels
        if (channels != 1) throw new Exception("Invalid Channels.");
        int bitrate = swapInt(dataStream.readInt());                // Bitrate
        if (bitrate != -1) throw new Exception("Invalid Bitrate.");
        samplesPerBlock = swapInt(dataStream.readInt());            // Frame Size
        if (mode == 0 && samplesPerBlock != 160 ||
            mode == 1 && samplesPerBlock != 320 ||
            mode == 2 && samplesPerBlock != 640) throw new Exception("Invalid Frame size.");
        int vbr = swapInt(dataStream.readInt());                    // Variable Bitrate
        if (vbr != 1) throw new Exception("Invalid Variable bitrate.");
        framesPerPacket = swapInt(dataStream.readInt());            // Frames per packet
        int extraHeader = swapInt(dataStream.readInt());            // Extra Header
        if (extraHeader != 0) throw new Exception("Invalid Extra header value.");
        int reserved1 = swapInt(dataStream.readInt());              // Reserved 1
        int reserved2 = swapInt(dataStream.readInt());              // Reserved 2
        
        decoder.init(mode, sampleRate, channels, true);
    }
    
    /**
     * Receives the header from the input stream
     * @param stream the input stream
     * @throws java.lang.Exception failed to receive header from stream
     */
    public void receiveHeaderFromStream(InputStream stream) throws Exception {
        OggHeader oggHeader;
        
        // Read the first header
        oggHeader = receiveOggHeaderFromStream(stream);
        if (oggHeader.headerType != OggHeader.BOS) throw new Exception("Invalid header.");
        //if (oggHeader.sequence != 0) throw new Exception("Invalid header.");
        if (oggHeader.segments != 1) throw new Exception("Invalid header.");
        if (oggHeader.segmentSize[0] != 80) throw new Exception("Invalid header.");
        receiveSpeexHeaderFromStream(stream);
        
        // Read the comment header
        oggHeader = receiveOggHeaderFromStream(stream);
        if (oggHeader.headerType != OggHeader.NONE) throw new Exception("Invalid header.");
        //if (oggHeader.sequence != 1) throw new Exception("Invalid header.");
        if (oggHeader.segments != 1) throw new Exception("Invalid header.");
        byte[] buffer = new byte[oggHeader.segmentSize[0]];
        receiveByteArrayFromStream(stream, buffer, 0, oggHeader.segmentSize[0]);
    }
    
    /**
     * Receives the audio data from the input stream
     * @param stream the input stream
     * @param fout the file output stream
     * @param synchronous true if the function will block until the transfer finishes
     * @throws java.lang.Exception failed to receive data from stream
     */
    public void receiveDataFromStream(InputStream stream, OutputStream fout, boolean synchronous) throws Exception {
        OggHeader oggHeader = new OggHeader();
        
        if (synchronous) {
            try {
                while (oggHeader.headerType != OggHeader.EOS) {
                    // Read the page header
                    oggHeader = receiveOggHeaderFromStream(stream);
                    if (oggHeader.headerType != OggHeader.NONE && oggHeader.headerType != OggHeader.EOS) throw new Exception("Invalid header.");

                    int length = 0;
                    for (int index = 0; index < oggHeader.segments; index++) length += oggHeader.segmentSize[index];
                    byte[] buffer = new byte[length];
                    receiveByteArrayFromStream(stream, buffer, 0, length);
                    
                    int offset = 0;
                    for (int index = 0; index < oggHeader.segments; index++) {
                        SpeexBlock block = new SpeexBlock(samplesPerBlock);
                        if (fout == null)
                            block.setEncodedData(buffer, offset, oggHeader.segmentSize[index]);
                        else {
                            fout.write(buffer, offset, oggHeader.segmentSize[index]);
                            block.setCache(cacheSize, oggHeader.segmentSize[index]);
                            cacheSize += oggHeader.segmentSize[index];
                        }
                        offset += oggHeader.segmentSize[index];

                        blockData.add(block);
                        availableBlocks++;
                        
                        if (listener != null) listener.update(getAvailable());
                    }
                }
            } catch (Throwable t) {}
            
            if (oggHeader.headerType != OggHeader.EOS) throw new Exception("Invalid audio data.");
        } else {
            if (transferThread == null) {
                transferThread = getTransferThread();
                transferThread.start(stream, fout);
            }
        }
    }
    
    /**
     * Receives the audio data from the input stream
     * @param stream the input stream
     * @param synchronous true if the function will block until the transfer finishes
     * @throws java.lang.Exception failed to receive data from stream
     */
    public synchronized void receiveFromStream(InputStream stream, boolean synchronous) throws Exception {
        FileOutputStream fout = null;
        if (cache != null) fout = new FileOutputStream(cache);
        
        receiveHeaderFromStream(stream);
        receiveDataFromStream(stream, fout, synchronous);
    }
    
    /**
     * Creates the transfer thread for data transfer
     * @return the transfer thread to be used
     */
    protected TransferThread getTransferThread() {
        return new SpeexTransferThread();
    }
    
    /**
     * Checks whether the current transfer buffer is filled
     * @param rate the current playback rate
     * @return true if the transfer buffer is filled
     */
    public synchronized boolean isTransferBuffered(float rate) {
        if (!isAvailable()) return false;
        if (!isTransferInProgress()) return true;
        
        return (getAvailable() - getTime()) * rate >= 30000;
    }
    
    /**
     * The transfer thread for speex data.
     */
    protected class SpeexTransferThread extends TransferThread {
        
        private long lastUpdatedTime = 0;
        
        /**
         * Starts the transfer of the speex data
         */
        public void run() {
            super.run();

            OggHeader oggHeader;
            
            try {
                while (inProgress) {
                    // Read the page header
                    oggHeader = receiveOggHeaderFromStream(in);
                    if (oggHeader.headerType != OggHeader.NONE && oggHeader.headerType != OggHeader.EOS) break;
                    
                    int length = 0;
                    for (int index = 0; index < oggHeader.segments; index++) length += oggHeader.segmentSize[index];
                    byte[] buffer = new byte[length];
                    receiveByteArrayFromStream(in, buffer, 0, length);

                    int offset = 0;
                    for (int index = 0; index < oggHeader.segments; index++) {
                        SpeexBlock block = new SpeexBlock(samplesPerBlock);
                        if (out == null)
                            block.setEncodedData(buffer, offset, oggHeader.segmentSize[index]);
                        else {
                            out.write(buffer, offset, oggHeader.segmentSize[index]);
                            block.setCache(cacheSize, oggHeader.segmentSize[index]);
                            cacheSize += oggHeader.segmentSize[index];
                        }
                        offset += oggHeader.segmentSize[index];

                        blockData.add(block);
                        availableBlocks++;
                        
                        if (listener != null) {
                            long time = new Date().getTime();
                            if (lastUpdatedTime == 0 || lastUpdatedTime + UPDATE_INTERVAL < time) {
                                listener.update(getAvailable());
                                lastUpdatedTime = time;
                            }
                        }
                    }
                    
                    if (oggHeader.headerType == OggHeader.EOS) break;
                }
            } catch (Throwable t) {}
            
            if (listener != null) listener.finish(getAvailable());
            
            transferThread = null;
        }
        
    }
    
    /**
     * The class stores an Ogg header.
     */
    protected class OggHeader {
        
        /**
         * Header type - none
         */
        public static final int NONE = 0;
        
        /**
         * Header type - continue
         */
        public static final int CONTINUE = 1;
        
        /**
         * Header type - begin of stream
         */
        public static final int BOS = 2;
        
        /**
         * Header type - end of stream
         */
        public static final int EOS = 4;
        
        /**
         * Header type
         */
        public int headerType = NONE;
        
        /**
         * Granule position
         */
        public long granulePos;
        
        /**
         * Stream serial
         */
        public int serial;
        
        /**
         * Sequence number
         */
        public int sequence;
        
        /**
         * Page checksum
         */
        public int checksum;
        
        /**
         * Page segment number
         */
        public int segments;
        
        /**
         * Segment size
         */
        public byte[] segmentSize;
        
    }
    
    /**
     * This class stores each block of speex data.
     */
    protected class SpeexBlock extends Block {
        
        /**
         * The location of the block in the cache file
         */
        protected long cacheOffset = -1;
        
        /**
         * The length of the block in the cache file
         */
        protected int cacheLength = 0;
        
        /**
         * Creates a new instance of SpeexBlock
         * @param size the size of the block
         */
        public SpeexBlock(int size) {
            super(size);
            reset();
        }
        
        /**
         * Sets cache information of the encoded data 
         * @param offset the offset of the encoded data
         * @param length the length of the data
         */
        public synchronized void setCache(long offset, int length) {
            data = null;
            cacheOffset = offset;
            cacheLength = length;
        }
        
        /**
         * Sets the encoded data in the block
         * @param encData the encoded data
         * @param length the length of the data
         */
        public synchronized void setEncodedData(byte[] encData, int offset, int length) {
            data = new byte[length];
            System.arraycopy(encData, offset, data, 0, length);
            cacheOffset = -1;
            cacheLength = 0;
        }
        
        /**
         * Reads a sample from the block
         * @throws java.lang.Exception failed to read the sample
         * @return the sample
         */
        public synchronized int read() throws Exception {
            if (eob()) throw new Exception("Invalid read request.");
            
            byte[] decodedBuffer = null;
            if (!decodedData.containsKey(this)) {
                if (data == null && cacheOffset < 0) throw new Exception("Invalid read request.");

                synchronized (decoder) {
                    if (data == null) {
                        byte[] cacheData = new byte[cacheLength];
                        cacheInputStream.read(cacheData, cacheOffset);
                        decoder.processData(cacheData, 0, cacheData.length);
                    }
                    else
                        decoder.processData(data, 0, data.length);
                    decodedBuffer = new byte[decoder.getProcessedDataByteSize()];
                    decoder.getProcessedData(decodedBuffer, 0);
                    
                    decodedData.clear();
                    decodedData.put(this, decodedBuffer);
                }
            } else decodedBuffer = (byte[]) decodedData.get(this);
            
            int sample = (int) ((decodedBuffer[2 * position + 1] << 8) | (decodedBuffer[2 * position] & 0xFF));
            position++;
            
            return sample;
        }
        
        private byte[] getDecodedBuffer() {
            byte[] decodedBuffer = null;
            
            if (!decodedData.containsKey(this)) {
                decodedBuffer = new byte[2 * size];
                Arrays.fill(decodedBuffer, (byte) 0);
                decodedData.clear();
                decodedData.put(this, decodedBuffer);
            } else decodedBuffer = (byte[]) decodedData.get(this);
            
            return decodedBuffer;
        }
        
        /**
         * Writes a sample to the block
         * @param sample the sample to write
         * @throws java.lang.Exception failed to write the sample
         */
        public synchronized void write(int sample) throws Exception {
            byte[] decodedBuffer = getDecodedBuffer();
            if (decodedBuffer == null) throw new Exception("Invalid write request.");
            
            decodedBuffer[2 * position] = (byte) (sample & 0xFF);
            decodedBuffer[2 * position + 1] = (byte) ((sample >> 8) & 0xFF);
            position++;
            
            if (position >= size) encodeData();
        }
        
        /**
         * Encodes the data into the encoded data buffer
         * @throws java.lang.Exception failed to encode the data
         */
        public void encodeData() throws Exception {
            byte[] decodedBuffer = getDecodedBuffer();
            if (decodedBuffer == null) throw new Exception("Invalid encoding request.");
            
            byte[] buffer = new byte[decodedBuffer.length];
            synchronized (encoder) {
                encoder.processData(decodedBuffer, 0, 2 * size);
                int size = encoder.getProcessedData(buffer, 0);
                
                setEncodedData(buffer, 0, size);
            }
        }
        
        /**
         * Creates a clone of the block
         * @return the clone of the block
         */
        public Object clone() {
            SpeexBlock block = new SpeexBlock(size);
            if (data != null) block.setEncodedData(data, 0, data.length);
            block.cacheOffset = cacheOffset;
            block.cacheLength = cacheLength;
            return block;
        }
        
        /**
         * Sends the block to an Ogg writer
         * @param writer the Ogg writer
         * @throws java.lang.Exception failed to send to the Ogg writer
         */
        public synchronized void sendToOggWriter(OggSpeexWriter writer) throws Exception {
            if (data == null && cacheOffset < 0) encodeData();
            if (data == null && cacheOffset < 0) throw new Exception("Invalid send request.");
            
            // data
            if (data == null) {
                byte[] cacheData = new byte[cacheLength];
                cacheInputStream.read(cacheData, cacheOffset);
                writer.writePacket(cacheData, 0, cacheData.length);
            }
            else
                writer.writePacket(data, 0, data.length);
        }
        
    }

}
