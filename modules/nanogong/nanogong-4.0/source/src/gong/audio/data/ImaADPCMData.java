/*
 * Copyright 2002-2008 The Gong Project (http://gong.ust.hk)
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
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ConnectException;
import java.util.Arrays;
import java.util.Date;
import java.util.Enumeration;
import javax.sound.sampled.AudioFormat;

/**
 * This class stores audio data using the Ima ADPCM codec.
 * @author Gibson Lam
 * @version 1.0, 26/09/2005
 */
public class ImaADPCMData extends BlockAudioData {
    
    /**
     * The extension used by ADPCM files
     */
    public static final String FILE_EXTENSION = ".wav";
    
    /**
     * The default ADPCM code size
     */
    protected static final int DEFAULT_ADPCM_CODE_SIZE = 4;
    
    /**
     * The code size
     */
    protected int codeSize = DEFAULT_ADPCM_CODE_SIZE;
    
    /**
     * The ADPCM index table
     */
    protected static final int[][] indexTable =
    {
        {-1,  2},
        {-1, -1,  2,  4},
        {-1, -1, -1, -1,  2,  4,  6,  8},
        {-1, -1, -1, -1, -1, -1, -1, -1,  1,  2,  4,  6,  8, 10, 13, 16}
    };
    
    /**
     * The ADPCM step size table
     */
    protected static final int[] stepsizeTable =
    {
        7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
        19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
        50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
        130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
        337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
        876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
        2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
        5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
        15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
    };
    
    /**
     * The default block alignment size
     */
    protected static final int DEFAULT_BLOCK_ALIGN = 256;
    
    /**
     * The block alignment size
     */
    protected int blockAlign = DEFAULT_BLOCK_ALIGN;
    
    /**
     * The number of bytes written to the cache
     */
    protected long cacheSize = 0;
    
    /**
     * Creates a new instance of ImaADPCMData
     */
    public ImaADPCMData() {
        super();
        samplesPerBlock = (blockAlign - 4) * 2 + 1;
    }
    
    /**
     * Creates a new instance of ImaADPCMData
     * @param format the audio format
     */
    public ImaADPCMData(AudioFormat format) {
        super(format);
        samplesPerBlock = (blockAlign - 4) * 2 + 1;
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
        int blockIndex = position / samplesPerBlock;
        if (blockIndex == 0) return new ImaADPCMBlock(samplesPerBlock);
        
        ImaADPCMBlock last = (ImaADPCMBlock) blockData.lastElement();
        ADPCMState state = (ADPCMState) last.getState();
        return new ImaADPCMBlock(state.index, samplesPerBlock);
    }
    
    /**
     * Gets the memory usage the audio data
     * @return the length of memory usage
     */
    public synchronized long getMemoryUsage() {
        return (long) blockData.size() * blockAlign;
    }
    
    /**
     * Creates a clone of this class
     * @return the clone of the audio data
     */
    public synchronized Object clone() {
        ImaADPCMData audioData = new ImaADPCMData(format);
        for (Enumeration en = blockData.elements(); en.hasMoreElements();) {
            audioData.blockData.add(((ImaADPCMBlock) en.nextElement()).clone());
        }
        audioData.blockAlign = blockAlign;
        audioData.samplesPerBlock = samplesPerBlock;
        audioData.availableBlocks = availableBlocks;
        
        return audioData;
    }
    
    /**
     * Sends the wav header to the output stream
     * @param stream the output stream
     * @throws java.lang.Exception failed to send header to stream
     */
    protected void sendHeaderToStream(OutputStream stream) throws Exception {
        DataOutputStream dataStream = new DataOutputStream(stream);
        
        // The header chunk
        dataStream.write(new String("RIFF").getBytes());                    // 0  - 3 : Chunk ID "RIFF"
        int chunkSize = 52 + blockData.size() * blockAlign;
        dataStream.writeInt(swapInt(chunkSize));                    // 4  - 7 : Chunk size
        dataStream.write(new String("WAVE").getBytes());                    // 8  - 11: Riff type "WAVE"
        dataStream.write(new String("fmt ").getBytes());                    // 12 - 15: Chunk ID "fmt "
        dataStream.writeInt(swapInt((int) 20));                     // 16 - 19: Chunk Size - 20
        dataStream.writeShort(swapShort((short) 17));               // 20 - 21: Compression Code - 17
        dataStream.writeShort(swapShort((short) 1));                // 22 - 23: Channel - 1
        dataStream.writeInt(swapInt((int) format.getSampleRate())); // 24 - 27: Sample Rate
        int bytesPerSecond = (int) (format.getSampleRate() / samplesPerBlock * blockAlign);
        dataStream.writeInt(swapInt(bytesPerSecond));               // 28 - 31: Bytes per Second
        dataStream.writeShort(swapShort((short) blockAlign));       // 32 - 33: Block Align
        dataStream.writeShort(swapShort((short) 4));                // 34 - 35: Sample Size
        dataStream.writeShort(swapShort((short) 2));                // 36 - 37: Extra Bytes
        dataStream.writeShort(swapShort((short) samplesPerBlock));  // 38 - 39: Samples per Block
        
        // The fact chunk
        dataStream.write(new String("fact").getBytes());                    // 0  - 3 : Chunk ID "fact"
        dataStream.writeInt(swapInt((int) 4));                      // 4  - 7 : Chunk size
        dataStream.writeInt(swapInt(getLength()));                  // 8  - 11: Length
    }
    
    /**
     * Sends the block data to the output stream
     * @param stream the output stream
     * @throws java.lang.Exception failed to send data to stream
     */
    protected void sendDataToStream(OutputStream stream) throws Exception {
        DataOutputStream dataStream = new DataOutputStream(stream);
        
        dataStream.write(new String("data").getBytes());                    // 0  - 3 : Chunk ID "data"
        int chunkSize = blockData.size() * blockAlign;
        dataStream.writeInt(swapInt(chunkSize));                    // 4  - 7 : Chunk size
        
        for (Enumeration en = blockData.elements(); en.hasMoreElements();) {
            ImaADPCMBlock block = (ImaADPCMBlock) en.nextElement();
            block.sendToStream(stream);
        }
    }
    
    /**
     * Sends the audio data to the output stream
     * @param stream the output stream
     * @throws java.lang.Exception failed to send data to stream
     */
    public synchronized void sendToStream(OutputStream stream) throws Exception {
        sendHeaderToStream(stream);
        sendDataToStream(stream);
    }
    
    /**
     * Receives the header from the input stream
     * @param stream the input stream
     * @param fout the file output stream
     * @throws java.lang.Exception failed to receive header from stream
     */
    public void receiveHeaderFromStream(InputStream stream, OutputStream fout) throws Exception {
        DataInputStream dataStream = new DataInputStream(stream);
        byte[] buffer = new byte[4];
        
        receiveByteArrayFromStream(dataStream, buffer, 0, 4);               // Chunk Id
        if (!(new String(buffer, 0, 4).equals("RIFF"))) throw new Exception("Invalid chunk id ('RIFF').");
        int riffSize = swapInt(dataStream.readInt());               // Chunk size
        receiveByteArrayFromStream(dataStream, buffer, 0, 4);               // Riff type
        if (!(new String(buffer, 0, 4).equals("WAVE"))) throw new Exception("Invalid riff type ('WAVE').");
        receiveByteArrayFromStream(dataStream, buffer, 0, 4);                   // Chunk ID
        if (!(new String(buffer, 0, 4).equals("fmt "))) throw new Exception("Invalid chunk id ('fmt ').");
        int chunkSize = swapInt(dataStream.readInt());              // Chunk Size
        if (chunkSize != 20) throw new Exception("Invalid chunk size.");
        short code = swapShort(dataStream.readShort());             // Compression Code
        if (code != 17) throw new Exception("Invalid compression code.");
        short channel = swapShort(dataStream.readShort());          // Channel
        if (channel != 1) throw new Exception("Invalid channel.");
        int sampleRate = swapInt(dataStream.readInt());             // Sampling Rate
        format = new AudioFormat(sampleRate, 16, 1, true, true);
        int bytesPerSec = swapInt(dataStream.readInt());            // Bytes per Second
        short blockAlign = swapShort(dataStream.readShort());       // Block Align
        this.blockAlign = blockAlign;
        short sampleSize = swapShort(dataStream.readShort());       // Sample Size
        if (sampleSize != 4) throw new Exception("Invalid sample size.");
        short extraBytes = swapShort(dataStream.readShort());       // Extra Bytes
        if (extraBytes != 2) throw new Exception("Invalid extra bytes.");
        short samplesPerBlock = swapShort(dataStream.readShort());  // Samples per Block
        this.samplesPerBlock = (blockAlign - 4) * 2 + 1;
        if (this.samplesPerBlock != samplesPerBlock) throw new Exception("Invalid samples per block.");
        
        if (fout != null) {
            DataOutputStream out = new DataOutputStream(fout);
            out.writeBytes("RIFF");
            out.writeInt(swapInt(riffSize));
            out.writeBytes("WAVE");
            out.writeBytes("fmt ");
            out.writeInt(swapInt(chunkSize));
            out.writeShort(swapShort(code));
            out.writeShort(swapShort(channel));
            out.writeInt(swapInt(sampleRate));
            out.writeInt(swapInt(bytesPerSec));
            out.writeShort(swapShort(blockAlign));
            out.writeShort(swapShort(sampleSize));
            out.writeShort(swapShort(extraBytes));
            out.writeShort(swapShort(samplesPerBlock));
            cacheSize += 40;
        }
    }
    
    /**
     * Receives the block data from the input stream
     * @param stream the input stream
     * @param fout the file output stream
     * @param synchronous true if the function will block until the transfer finishes
     * @throws java.lang.Exception failed to receive block data from stream
     */
    protected void receiveDataFromStream(InputStream stream, OutputStream fout, boolean synchronous) throws Exception {
        DataInputStream dataStream = new DataInputStream(stream);
        byte[] buffer = new byte[4];
        int chunkSize;
        
        while (true) {
            receiveByteArrayFromStream(dataStream, buffer, 0, 4);           // Chunk Id
            chunkSize = swapInt(dataStream.readInt());              // Chunk size
            if (new String(buffer, 0, 4).equals("data"))
                break;
            else {
                if (fout == null)
                    dataStream.skip(chunkSize);
                else {
                    byte[] chunkData = new byte[chunkSize];
                    dataStream.read(chunkData);
                    
                    DataOutputStream out = new DataOutputStream(fout);
                    out.write(buffer, 0, 4);
                    out.writeInt(swapInt(chunkSize));
                    out.write(chunkData);
                    cacheSize += 8 + chunkSize;
                }
            }
        }
        if (chunkSize % blockAlign != 0) throw new Exception("Invalid block align.");
        
        if (fout != null) {
            DataOutputStream out = new DataOutputStream(fout);
            out.writeBytes("data");
            out.writeInt(swapInt(chunkSize));
            cacheSize += 8;
        }
        
        // Create the blocks
        for (int index = 0; index < (int) (chunkSize / blockAlign); index++) {
            ImaADPCMBlock block = new ImaADPCMBlock(samplesPerBlock);
            blockData.add(block);
        }
        
        if (synchronous) {
            try {
                while (true) {
                    ImaADPCMBlock block = (ImaADPCMBlock) blockData.get(availableBlocks);
                    block.receiveFromStream(stream, fout);
                    availableBlocks++;
                    
                    if (listener != null) listener.update(getAvailable());
                    
                    try {
                        if (fout != null) fout.close();
                    } catch (IOException ex) {}
                }
            } catch (Throwable t) {}
            
            if (chunkSize != blockData.size() * blockAlign) throw new Exception("Invalid block data.");
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
        
        receiveHeaderFromStream(stream, fout);
        receiveDataFromStream(stream, fout, synchronous);
    }
    
    /**
     * Creates the transfer thread for data transfer
     * @return the transfer thread to be used
     */
    protected TransferThread getTransferThread() {
        return new ImaADPCMTransferThread();
    }
    
    /**
     * The transfer thread of the audio data.
     */
    protected class ImaADPCMTransferThread extends TransferThread {
        
        private long lastUpdatedTime = 0;
        
        /**
         * Starts to transfer the ADPCM data
         */
        public void run() {
            super.run();
            
            try {
                while (inProgress) {
                    ImaADPCMBlock block = (ImaADPCMBlock) blockData.get(availableBlocks);
                    block.receiveFromStream(in, out);
                    availableBlocks++;
                    
                    if (listener != null) {
                        long time = new Date().getTime();
                        if (lastUpdatedTime == 0 || lastUpdatedTime + UPDATE_INTERVAL < time) {
                            listener.update(getAvailable());
                            lastUpdatedTime = time;
                        }
                    }
                }
            } catch (Exception ex) {}
            
            if (listener != null) listener.finish(getAvailable());
            
            try {
                if (out != null) out.close();
            } catch (IOException ex) {}
            
            transferThread = null;
        }
        
    }
    
    /**
     * This class stores a block of ADPCM data.
     */
    protected class ImaADPCMBlock extends Block {
        
        /**
         * The state in the header
         */
        protected ADPCMState header = new ADPCMState();
        /**
         * The current state
         */
        protected ADPCMState state = null;
        
        /**
         * The location of the block in the cache file
         */
        protected long cacheOffset = -1;
        
        /**
         * Creates a new instance of ImaADPCMBlock
         * @param size the block size
         */
        public ImaADPCMBlock(int size) {
            this(0, size);
        }
        
        /**
         * Creates a new instance of ImaADPCMBlock
         * @param index the initial index
         * @param size the block size
         */
        public ImaADPCMBlock(int index, int size) {
            super(size);
            header.index = index;
        }
        
        /**
         * Gets the current state
         * @return the current state
         */
        public synchronized ADPCMState getState() {
            return state;
        }
        
        /**
         * Decodes a ADPCM delta code
         * @param deltaCode the delta code
         * @return the decoded sample
         */
        protected int decode(byte deltaCode) {
            int step = stepsizeTable[state.index];
            
            // Construct difference
            int diff = step >> (codeSize - 1);
            int mask = 1 << (codeSize - 2);
            for (int index = 0; index < codeSize - 1; index++, mask >>= 1) {
                if ((deltaCode & mask) != 0) diff += step >> index;
            }
            mask = 1 << (codeSize - 1);
            if ((deltaCode & mask) != 0) diff = -diff;
            
            // Build new sample
            state.previousValue += diff;
            if (state.previousValue > 32767) state.previousValue = 32767;
            else if (state.previousValue < -32768) state.previousValue = -32768;
            
            // Update step
            state.index += indexTable[codeSize - 2][(int) (deltaCode & ~mask)];
            if (state.index < 0) state.index = 0;
            else if (state.index > 88) state.index = 88;
            
            return state.previousValue;
        }
        
        /**
         * Encodes a sample to a ADPCM delta code
         * @param sample the sample
         * @return the delta code
         */
        protected byte encode(int sample) {
            int diff = sample - state.previousValue;
            int step = stepsizeTable[state.index];
            byte deltaCode = 0;
            
            // Set sign bit
            if (diff < 0) {
                deltaCode = (byte) (1 << (codeSize - 1));
                diff = -diff;
            }
            
            // deltaCode = (diff<<2)/step
            for (int index = codeSize - 2; index >= 0; index--, step >>= 1) {
                if (diff >= step) {
                    deltaCode |= (byte) (1 << index);
                    diff -= step;
                }
            }
            
            // Update state
            decode(deltaCode);
            
            return deltaCode;
        }
        
        /**
         * Reads a sample from the block
         * @throws java.lang.Exception failed to read a sample
         * @return the sample
         */
        public synchronized int read() throws Exception {
            if (eob()) throw new Exception("Invalid read request.");
            
            if (position == 0) {
                position = 1;
                state = (ADPCMState) header.clone();
                return state.previousValue;
            }
            
            if (data == null && cacheOffset < 0) throw new Exception("Invalid read request.");
            
            byte deltaCode;
            
            int dataIndex = (position - 1) / 2;
            int byteOrder = (position - 1) % 2;
            
            byte byteData = 0;
            if (data == null)
                byteData = (byte) cacheInputStream.read(cacheOffset + dataIndex + 4);
            else
                byteData = data[dataIndex];
            
            if (byteOrder == 0)
                deltaCode = (byte) (byteData & 0xF);
            else
                deltaCode = (byte) ((byteData >> 4) & 0xF);
            position++;
            
            return decode(deltaCode);
        }
        
        /**
         * Writes a sample to the block
         * @param sample the sample to be written
         * @throws java.lang.Exception failed to write the sample
         */
        public synchronized void write(int sample) throws Exception {
            if (eob()) throw new Exception("Invalid write request.");
            
            if (data == null) {
                data = new byte[(size - 1) / 2];
                Arrays.fill(data, (byte) 0);
            }
            
            if (position == 0) {
                header.previousValue = sample;
                state = (ADPCMState) header.clone();
                position = 1;
            } else {
                byte deltaCode = encode(sample);
                
                int dataIndex = (position - 1) / 2;
                int byteOrder = (position - 1) % 2;
                if (byteOrder == 0)
                    data[dataIndex] |= (byte) (deltaCode & 0xF);
                else
                    data[dataIndex] |= (byte) ((deltaCode << 4) & 0xF0);
                position++;
            }
        }
        
        /**
         * Seeks to a particular position in the block
         * @param position the position
         * @throws java.lang.Exception failed to seek to the position
         * @return the new position
         */
        public synchronized int seek(int position) throws Exception {
            if (position < 0 || position >= size) throw new Exception("Invalid seek position.");
            if (position == this.position) return position;
            
            if (position == 0) {
                reset();
                return position;
            }
            
            if (position < this.position) {
                reset();
                
                for (int index = 0; index < position; index++) read();
                return (this.position = position);
            } else {
                for (int index = this.position; index < position; index++) read();
                return (this.position = position);
            }
        }
        
        /**
         * Clears the block data
         */
        public synchronized void clear() {
            super.clear();
            
            header.index = 0;
            header.previousValue = 0;
        }
        
        /**
         * Resets the state of the block
         */
        public synchronized void reset() {
            super.reset();
            state = null;
        }
        
        /**
         * Creates a clone of the block
         * @return the clone of the block
         */
        public Object clone() {
            ImaADPCMBlock block = new ImaADPCMBlock(size);
            if (data != null) {
                block.data = new byte[data.length];
                System.arraycopy(data, 0, block.data, 0, data.length);
            }
            block.header = (ADPCMState) header.clone();
            block.size = size;
            block.position = position;
            block.cacheOffset = cacheOffset;
            return block;
        }
        
        /**
         * Sends the block to an output stream
         * @param stream the output stream
         * @throws java.lang.Exception failed to send the block
         */
        public synchronized void sendToStream(OutputStream stream) throws Exception {
            if (data == null && cacheOffset < 0) throw new Exception("Invalid send request.");
            
            DataOutputStream dataStream = new DataOutputStream(stream);
            
            // header
            dataStream.writeShort(swapShort((short) header.previousValue));
            dataStream.writeByte((byte) header.index);
            dataStream.writeByte(0);
            
            // data
            if (data == null) {
                byte[] buffer = new byte[(size - 1) / 2];
                cacheInputStream.read(buffer, cacheOffset + 4);
                dataStream.write(buffer);
            } else
                dataStream.write(data);
        }
        
        /**
         * Receives a block from an input stream
         * @param stream the input stream
         * @param fout the cache file output
         * @throws java.lang.Exception failed to receive the block data
         */
        public synchronized void receiveFromStream(InputStream stream, OutputStream fout) throws Exception {
            DataInputStream dataStream = new DataInputStream(stream);
            
            // header
            header.previousValue = (int) swapShort(dataStream.readShort());
            header.index = (int) dataStream.read();
            if (header.index == -1) throw new ConnectException("Connection error.");
            if (dataStream.read() == -1) throw new ConnectException("Connection error.");
            
            // data
            data = new byte[(size - 1) / 2];
            receiveByteArrayFromStream(dataStream, data, 0, data.length);
            
            if (cache != null && fout != null) {
                cacheOffset = cacheSize;
                
                DataOutputStream out = new DataOutputStream(fout);
                out.writeShort(swapShort((short) header.previousValue));
                out.write(header.index);
                out.write(0);
                out.write(data, 0, data.length);
                cacheSize += data.length + 4;
                
                data = null;
            }
        }
        
    }
    
    /**
     * This class defines a ADPCM state.
     */
    protected class ADPCMState implements Cloneable {
        
        /**
         * The previous sample value
         */
        public int previousValue;
        /**
         * The initial index
         */
        public int index;
        
        /**
         * Creates a new instance of ADPCMState
         */
        public ADPCMState() {
            previousValue = 0;
            index = 0;
        }
        
        /**
         * Creates a new instance of ADPCMState
         * @param sample the previous sample value
         * @param index the initial index
         */
        public ADPCMState(int sample, int index) {
            previousValue = sample;
            this.index = index;
        }
        
        /**
         * Creates a clone of the state
         * @return the clone of the state
         */
        public Object clone() {
            return new ADPCMState(previousValue, index);
        }
        
    }
    
}
