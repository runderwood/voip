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
 
package gong.audio;

import gong.event.AudioDataListener;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ConnectException;
import java.util.Date;
import javax.sound.sampled.AudioFormat;
import gong.audio.data.ImaADPCMData;
import gong.audio.data.SpeexData;
import java.io.File;
import java.io.FileInputStream;

/**
 * This abstract audio data class defines the interface of a data container of
 * an audio recording.
 * @author Gibson Lam
 * @version 3.0, 12/08/2008
 * @version 4.0, 18/01/2010
 */
public abstract class AudioData implements Cloneable {
    
    /**
     * The extension used by audio files
     */
    public static final String FILE_EXTENSION = ".raw";
    
    /**
     * The default stream connection timeout
     */
    protected static final int STREAM_TIMEOUT = 60000;
    
    /**
     * The default interval for listener update
     */
    protected static final int UPDATE_INTERVAL = 500;
    
    /**
     * The audio format of the data
     */
    protected AudioFormat format = null;
    
    /**
     * The current sample position
     */
    protected int position = 0;
    
    /**
     * The asynchronous download thread
     */
    protected TransferThread transferThread = null;
    
    /**
     * The audio data listner
     */
    protected AudioDataListener listener = null;
    
    /**
     * The file cache
     */
    protected File cache = null;
    
    /**
     * The cache reader
     */
    protected CacheInputStream cacheInputStream = new CacheInputStream();
    
    /**
     * Creates a new instance of AudioData
     */
    public AudioData() {
    }
    
    /**
     * Creates a new instance of AudioData
     * @param format the audio format
     */
    public AudioData(AudioFormat format) {
        this.format = format;
    }
    
    /**
     * Returns the audio format of the data
     * @return the audio format
     */
    public AudioFormat getFormat() {
        return format;
    }
    
    /**
     * Returns the file extension of the data
     * @return the file extension
     */
    abstract public String getFileExtension();
    
    /**
     * Sets a audio data listener
     * @param listener The audio data listener.
     */
    public synchronized void setListener(AudioDataListener listener) {
        this.listener = listener;
        if (listener != null) {
            if (isTransferInProgress())
                listener.update(getAvailable());
            else
                listener.finish(getAvailable());
        }
    }
    
    /**
     * Sets the file cache
     * @param file the file
     */
    public void setCache(File file) {
        cache = file;
    }
    
    /**
     * Gets the file cache
     * @return the file
     */
    public File getCache() {
        return cache;
    }
    
    /**
     * Clears the file cache */
    public void clearCache() {
        if (isTransferInProgress()) stopTransfer();
        if (cache != null) {
            try {
                cacheInputStream.close();
            } catch (IOException ex) {}
            cache.delete();
        }
    }
    
    /**
     * Reads a sample from the audio data
     * @return the sample value
     * @throws java.lang.Exception failed to read sample
     */
    abstract public int read() throws Exception;
    
    /**
     * Reads a set of samples from the audio data
     * @param buffer the sample buffer
     * @param offset the offset in the buffer
     * @param length the length of the samples to be read
     * @return the number of samples read
     * @throws java.lang.Exception failed to read sample
     */
    public synchronized int read(int[] buffer, int offset, int length) throws Exception {
        int sampleRead = 0;
        for (int index = offset; index < offset + length; index++) {
            try {
                buffer[index] = read();
                sampleRead++;
            } catch (Throwable t) {
                break;
            }
        }
        return sampleRead;
    }
    
    /**
     * Writes a sample to the audio data
     * @param sample the sample value
     * @throws java.lang.Exception failed to write sample
     */
    abstract public void write(int sample) throws Exception;
    
    /**
     * Seeks to the given position in the audio data
     * @param position the position of the sample
     * @return the position after seeking
     * @throws java.lang.Exception failed to seek to the given position
     */
    abstract public int seek(int position) throws Exception;
    
    /**
     * Seeks to the given position and reads the audio data without changing position
     * @param position the position of the sample
     * @return the sample at the seek position
     * @throws java.lang.Exception failed to seek and read the given position
     */
    public synchronized int seekAndRead(int position) throws Exception {
        int mark = this.position;
        seek(position);
        int sample = read();
        seek(mark);
        return sample;
    }
    
    /**
     * Seeks to the given position and reads a set of audio data without changing position
     * @param position the position of the sample
     * @param buffer the sample buffer
     * @param offset the offset in the buffer
     * @param length the length of the samples to be read
     * @return the number of sample read
     * @throws java.lang.Exception failed to seek and read the given position
     */
    public synchronized int seekAndRead(int position, int[] buffer, int offset, int length) throws Exception {
        int mark = this.position;
        seek(position);
        int sampleRead = read(buffer, offset, length);
        seek(mark);
        return sampleRead;
    }
    
    /**
     * Skips a certain distance from the current position
     * @param size the skipped distance
     * @return the number of actual samples skipped
     * @throws java.lang.Exception failed to skip by the given size
     */
    public synchronized int skip(int size) throws Exception {
        return seek(position + size);
    }
    
    /**
     * Resets the audio data position
     */
    public synchronized void reset() {
        position = 0;
    }
    
    /**
     * Gets the position of the data
     * @return the current position
     */
    public synchronized int getPosition() {
        return position;
    }
    
    /**
     * Checks whether there is sample available in the next read request
     * @return true if sample is available
     */
    abstract public boolean isAvailable();
    
    /**
     * Sets the media time of the audio data to the appropriate position
     * @param time the media time
     * @throws java.lang.Exception failed to set the media time
     */
    public synchronized void setTime(long time) throws Exception {
        int position = (int) ((double) time / 1000D * format.getSampleRate());
        if (position < 0) position = 0;
        if (position > getLength()) position = getLength();
        seek(position);
    }
    
    /**
     * Gets the media time of the audio data from the current position
     * @return the media time
     */
    public synchronized long getTime() {
        long time = (long) ((double) position / format.getSampleRate() * 1000D);
        if (time < 0) time = 0;
        if (time > getDuration()) time = getDuration();
        return time;
    }
    
    /**
     * Gets the media duration of the audio data
     * @return the media duration
     */
    public synchronized long getDuration() {
        return (long) ((double) getLength() / format.getSampleRate() * 1000D);
    }
    
    /**
     * Gets the available data in media duration
     * @return the available duration
     */
    abstract public long getAvailable();
    
    /**
     * Gets the length of samples in the audio data
     * @return the length of samples
     */
    abstract public int getLength();
    
    /**
     * Gets the memory usage the audio data
     * @return the length of memory usage
     */
    abstract public long getMemoryUsage();
    
    /**
     * Creates a clone of this class
     * @return the clone object
     */
    public Object clone() {
        return this;
    }
    
    /**
     * Closes the audio data
     * @throws java.lang.Exception failed to close the audio data
     */
    public void close() throws Exception {
    }
    
    /**
     * Sends the audio data to the output stream
     * @param stream the output stream
     * @throws java.lang.Exception failed to send data to stream
     */
    abstract public void sendToStream(OutputStream stream) throws Exception;
    
    /**
     * Receives the audio data from the input stream
     * @param stream the input stream
     * @param synchronous whether the function will block until the transfer finishes
     * @throws java.lang.Exception failed to receive data from stream
     */
    abstract public void receiveFromStream(InputStream stream, boolean synchronous) throws Exception;
    
    /**
     * A custom thread for reading the socket */
    protected class StreamReader extends Thread {
        
        private InputStream stream;
        private byte[] buffer;
        private int offset;
        private int length;
        private int bytesRead;
        private boolean closed;
        
        /**
         * Creates a new instance of StreamReader
         * @param stream the input stream
         * @param buffer the data buffer
         * @param offset the offset in the buffer
         * @param length the length of the data
         */
        public StreamReader(InputStream stream, byte[] buffer, int offset, int length) {
            this.stream = stream;
            this.buffer = buffer;
            this.offset = offset;
            this.length = length;
            bytesRead = 0;
            closed = false;
        }
        
        /**
         * Starts the stream reader
         */
        public synchronized void run() {
            try {
                while (!closed && bytesRead < length) bytesRead += stream.read(buffer, offset + bytesRead, length - bytesRead);
            } catch (IOException e) {
                e.printStackTrace();
            }
            notifyAll();
        }
        
        /**
         * Closes the stream reader
         */
        public void close() {
            closed = true;
        }
        
        /**
         * Returns the number of bytes available for reading
         * @return the number of available bytes remaining
         */
        public int getBytesRemaining() {
            return (length - bytesRead);
        }
        
    }
    
    /**
     * Receives byte array from the input stream with timeout
     * @param stream the input stream
     * @param buffer the byte array
     * @param offset the offset
     * @param length the length
     * @throws java.io.IOException failed to receive data from stream
     * @throws java.net.ConnectException failed to connect to the stream
     */
    protected void receiveByteArrayFromStream(InputStream stream, byte[] buffer, int offset, int length) throws IOException, ConnectException {
        StreamReader reader = new StreamReader(stream, buffer, offset, length);
        synchronized (reader) {
            reader.start();
            while (reader.getBytesRemaining() > 0) {
                try {
                    reader.wait(STREAM_TIMEOUT);
                } catch (InterruptedException e) {}
                if (reader.getBytesRemaining() > 0) {
                    reader.close();
                    throw new ConnectException("Connection timeout.");
                }
            }
        }
    }
    
    /**
     * Creates a suitable audio data from the input stream
     * @param stream the input stream
     * @return the audio data of the appropriate type
     * @throws java.lang.Exception failed to create from stream
     */
    static public AudioData createFromStream(InputStream stream) throws Exception {
        try {
            // test ADPCM
            ImaADPCMData adpcm = new ImaADPCMData();
            stream.mark(0);
            try {
                adpcm.receiveHeaderFromStream(stream, null);
                stream.reset();
                return adpcm;
            } catch (Exception ex) {
            }
            stream.reset();
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        try {
            // test Speex
            SpeexData speex = new SpeexData();
            stream.mark(0);
            try {
                speex.receiveHeaderFromStream(stream);
                stream.reset();
                return speex;
            } catch (Exception ex) {
            }
            stream.reset();
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        return null;
    }
    
    /**
     * Creates the transfer thread for data transfer
     * @return the transfer thread to be used
     */
    protected TransferThread getTransferThread() {
        return new TransferThread();
    }
    
    /**
     * Checks whether the current transfer buffer is filled
     * @param rate the current playback rate
     * @return true if the transfer buffer is filled
     */
    public synchronized boolean isTransferBuffered(float rate) {
        if (!isAvailable()) return false;
        if (!isTransferInProgress()) return true;
        
        // under or on 1 minute, wait for the whole buffer; otherwise download 30 sec only
        if (getDuration() <= 60000) {
            long transferTime = transferThread.getTime();
            long remaining = getDuration() - getAvailable();
            
            return ((double) remaining * (transferTime + 1000D) / (double) getAvailable()) * rate * 1.1D <= (getDuration() - getTime());
        } else
            return ((getAvailable() - getTime()) * rate >= 30000 || getAvailable() == getDuration());
    }
    
    /**
     * Checks whether the audio data is being transferred
     * @return true if the audio data is being transferred
     */
    public synchronized boolean isTransferInProgress() {
        return (transferThread != null);
    }
    
    /**
     * Stops the current transfer
     */
    public synchronized void stopTransfer() {
        if (isTransferInProgress()) {
            transferThread.kill();
            transferThread = null;
        }
    }
    
    /**
     * The class template for the transfer thread
     */
    protected class TransferThread extends Thread {
        
        /**
         * The input stream
         */
        protected InputStream in = null;
        /**
         * The output stream
         */
        protected OutputStream out = null;
        private long startTime;
        /**
         * True if the thread is in progress
         */
        protected boolean inProgress = true;
        
        /**
         * Starts the transfer
         * @param in the input stream
         * @param out the output stream
         */
        public void start(InputStream in, OutputStream out) {
            this.in = in;
            this.out = out;
            start();
        }
        
        /**
         * Records the start time of the transfer
         */
        protected void recordStartTime() {
            startTime = (new Date().getTime());
        }
        
        /**
         * Gets the elapsed time
         * @return the elapsed time
         */
        public long getTime() {
            return (new Date().getTime()) - startTime;
        }
        
        /**
         * Starts the transfer
         */
        public void run() {
            recordStartTime();
        }
        
        /**
         * Kills the transfer
         */
        public void kill() {
            try {
                in.close();
            } catch (Exception ex) {}
            try {
                out.close();
            } catch (Exception ex) {}
            inProgress = false;
        }
        
    }
    
    /**
     * A class to read the cache file.
     */
    protected class CacheInputStream {
        
        private FileInputStream stream;
        private long position;
        private byte data;
        
        /**
         * Creates a new instance of CacheInputStream
         */
        public CacheInputStream() {
            stream = null;
            position = -1;
        }
        
        /**
         * Reads a buffer from the cache input stream
         * @param data the data buffer
         * @param pos the position to start reading
         * @throws java.io.IOException failed to read the cache
         */
        public void read(byte[] data, long pos) throws IOException {
            for (int index = 0; index < data.length; index++) data[index] = read(index + pos);
        }
        
        /**
         * Reads a single sample from the cache
         * @param pos the position
         * @return the sample
         * @throws java.io.IOException failed to read the sample
         */
        public byte read(long pos) throws IOException {
            if (pos == position) return data;
            if (stream == null || pos < position) {
                if (stream != null) stream.close();
                stream = new FileInputStream(cache);
            }
            if (position == -1 || pos < position)
                stream.skip(pos);
            else if (pos - position > 1)
                stream.skip(pos - position - 1);
            data = (byte) stream.read();
            position = pos;
            
            return data;
        }
        
        /**
         * Closes the cache input stream
         * @throws java.io.IOException failed to close the stream
         */
        public void close() throws IOException {
            if (stream != null) stream.close();
        }
        
    }
    
    /**
     * Swaps the bytes in the Long Integer (8 bytes)
     * @param i the long integer
     * @return the swapped long integer
     */
    public static long swapLong(long i) {
        long byte0 = i & 0xff;
        long byte1 = (i >> 8) & 0xff;
        long byte2 = (i >> 16) & 0xff;
        long byte3 = (i >> 24) & 0xff;
        long byte4 = (i >> 32) & 0xff;
        long byte5 = (i >> 40) & 0xff;
        long byte6 = (i >> 48) & 0xff;
        long byte7 = (i >> 56) & 0xff;
        
        // swap the byte order
        return (byte0 << 56) | (byte1 << 48) | (byte2 << 40) | (byte3 << 32) | (byte4 << 24) | (byte5 << 16) | (byte6 << 8) | byte7;
    }
    
    /**
     * Swaps the bytes in the Integer (4 bytes)
     * @param i the integer
     * @return the swapped integer
     */
    public static int swapInt(int i) {
        int byte0 = i & 0xff;
        int byte1 = (i >> 8) & 0xff;
        int byte2 = (i >> 16) & 0xff;
        int byte3 = (i >> 24) & 0xff;
        
        // swap the byte order
        return (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
    }
    
    /**
     * Swaps the bytes in the Short (2 bytes)
     * @param i the short integer
     * @return the swapped short integer
     */
    public static short swapShort(short i) {
        int byte0 = i & 0xff;
        int byte1 = (i >> 8) & 0xff;
        
        // swap the byte order
        return (short) ((byte0 << 8) | byte1);
    }
    
}
