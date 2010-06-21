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

import gong.audio.AudioData;
import java.util.Arrays;
import java.util.Vector;
import javax.sound.sampled.AudioFormat;

/**
 * This is a template class for storing block audio data.
 * @author Gibson Lam
 * @version 1.0, 28/09/2005
 */
public abstract class BlockAudioData extends AudioData {
    
    /**
     * Data blocks of the audio
     */
    protected Vector blockData = new Vector();
    
    /**
     * The number of samples for each block
     */
    protected int samplesPerBlock;
    
    /**
     * Blocks available in the data
     */
    protected int availableBlocks;
    
    /**
     * Creates a new instance of BlockAudioData
     */
    public BlockAudioData() {
        super();
    }
    
    /**
     * Creates a new instance of BlockAudioData
     * @param format the audio format
     */
    public BlockAudioData(AudioFormat format) {
        super(format);
    }
    
    /**
     * Reads a sample from the audio data
     * @return the sample value
     * @throws java.lang.Exception failed to read sample
     */
    public synchronized int read() throws Exception {
        if (blockData.size() == 0) throw new Exception("Data not available.");
        
        int blockIndex = position / samplesPerBlock;
        if (blockIndex >= blockData.size()) throw new Exception("Buffer overflow.");
        if (blockIndex >= availableBlocks) throw new Exception("Data not available.");
        
        Block block = (Block) blockData.get(blockIndex);
        block.seek(position % samplesPerBlock);
        position++;
        
        return block.read();
    }
    
    /**
     * Reads a set of samples from the audio data
     * @param buffer the sample buffer
     * @param offset the offset in the buffer
     * @param length the length of the samples to be read
     * @return the number of samples read
     * @throws java.lang.Exception failed to read sample
     */
    public synchronized int read(int[] buffer, int offset, int length) throws Exception {
        if (blockData.size() == 0) throw new Exception("Data not available.");
        
        int blockIndex = position / samplesPerBlock;
        if (blockIndex >= blockData.size()) throw new Exception("Buffer overflow.");
        if (blockIndex >= availableBlocks) throw new Exception("Data not available.");
        
        Block block = (Block) blockData.get(blockIndex);
        block.seek(position % samplesPerBlock);
        
        int lastPos = position;
        for (int index = offset; index < offset + length; index++) {
            try {
                buffer[index] = block.read();
                position++;
                
                if (block.eob()) {
                    blockIndex++;
                    if (blockIndex >= blockData.size()) throw new Exception("Buffer overflow.");
                    if (blockIndex >= availableBlocks) throw new Exception("Data not available.");
                    
                    block = (Block) blockData.get(blockIndex);
                    block.seek(0);
                }
            } catch (Throwable t) {
                break;
            }
        }
        
        return (position - lastPos);
    }
    
    /**
     * Creates a new block
     * @return the new block
     */
    protected abstract Block createBlock();
    
    /**
     * Writes a sample to the audio data
     * @param sample the sample value
     * @throws java.lang.Exception failed to write sample
     */
    public synchronized void write(int sample) throws Exception {
        Block block;
        int blockIndex = position / samplesPerBlock;
        if (blockIndex >= blockData.size()) {
            block = createBlock();
            blockData.add(block);
            availableBlocks = blockData.size();
            
            if (listener != null) listener.update(getAvailable());
        } else block = (Block) blockData.get(blockIndex);
        
        block.write(sample);
        position++;
    }
    
    /**
     * Seeks to the given position in the audio data
     * @param position the position of the sample
     * @throws java.lang.Exception failed to seek to the given position
     * @return the new position
     */
    public synchronized int seek(int position) throws Exception {
        int blockIndex = position / samplesPerBlock;
        if (blockIndex < 0 || blockIndex >= blockData.size()) throw new Exception("Invalid seek position.");
        
        Block block = (Block) blockData.get(blockIndex);
        block.seek(position % samplesPerBlock);
        
        return (this.position = position);
    }
    
    /**
     * Checks whether there is sample available in the next read request
     * @return true if sample is available
     */
    public synchronized boolean isAvailable() {
        if (blockData.size() == 0) return false;
        
        int blockIndex = position / samplesPerBlock;
        if (blockIndex >= blockData.size()) return false;
        if (blockIndex >= availableBlocks) return false;
        
        return true;
    }
    
    /**
     * Gets the available data in media duration
     * @return the available duration
     */
    public synchronized long getAvailable() {
        return (long) ((double) (availableBlocks * samplesPerBlock) / format.getSampleRate() * 1000D);
    }
    
    /**
     * Gets the length of samples in the audio data
     * @return the length of samples
     */
    public synchronized int getLength() {
        return blockData.size() * samplesPerBlock;
    }
    
    /**
     * Gets the index of the block of the media time
     * @param time the media time
     * @return the index of block
     */
    public synchronized int getBlockIndex(long time) {
        int position = (int) ((double) time / 1000D * format.getSampleRate());
        int blockIndex = position / samplesPerBlock;
        if (blockIndex < 0) blockIndex = 0;
        if (blockIndex > blockData.size()) blockIndex = blockData.size();
        return blockIndex;
    }
    
    /**
     * Gets the media time of the block
     * @param index the block index
     * @return the media time of block
     */
    public synchronized long getBlockTime(int index) {
        int position = index * samplesPerBlock;
        return (long) ((double) position / format.getSampleRate() * 1000D);
    }
    
    /**
     * Gets the number of the blocks in the audio data
     * @return the number of blocks
     */
    public synchronized int getBlockSize() {
        return blockData.size();
    }
    
    /**
     * Deletes part of the blocks of the audio data
     * @param start the start index (inclusive)
     * @param end the end index (exclusive)
     * @return the deleted audio data
     */
    public synchronized BlockAudioData delete(int start, int end) {
        Vector backup = blockData;
        
        blockData = new Vector();
        availableBlocks = 0;
        BlockAudioData deletedData = (BlockAudioData) clone();
        
        deletedData.blockData = new Vector(backup.subList(start, end));
        deletedData.availableBlocks = end - start;
        
        backup.subList(start, end).clear();
        blockData = backup;
        availableBlocks = blockData.size();
        
        return deletedData;
    }
    
    /**
     * Inserts blocks into the audio data
     * @param audioData the data to be inserted
     * @param start the start index
     */
    public synchronized void insert(BlockAudioData audioData, int start) {
        if (!this.getClass().isInstance(audioData)) return;
        BlockAudioData insertedData = (BlockAudioData) audioData.clone();
        blockData.addAll(start, insertedData.blockData);
        availableBlocks = blockData.size();
    }
    
    /**
     * This class stores a block of audio data.
     */
    protected class Block implements Cloneable {
        
        /**
         * The data buffer
         */
        protected byte[] data = null;
        /**
         * The size of the buffer
         */
        protected int size = 0;
        /**
         * The current position of the block
         */
        protected int position = 0;
        
        /**
         * Creates a new instance of Block
         * @param size the block size
         */
        public Block(int size) {
            this.size = size;
        }
        
        /**
         * Gets the data buffer from the block
         * @return the data buffer
         */
        public synchronized byte[] getData() {
            return data;
        }
        
        /**
         * Gets the size of the block
         * @return the block size
         */
        public synchronized int getSize() {
            return size;
        }
        
        /**
         * Gets the position of the block
         * @return the current position
         */
        public synchronized int getPosition() {
            return position;
        }
        
        /**
         * Reads a sample from the block
         * @return the sample
         * @throws java.lang.Exception failed to read a sample
         */
        public int read() throws Exception {
            return 0;
        }
        
        /**
         * Writes a sample to the block
         * @param sample the sample to be written
         * @throws java.lang.Exception failed to write the sample
         */
        public void write(int sample) throws Exception {
        }
        
        /**
         * Seeks to a particular position in the block
         * @param position the position
         * @return the new position
         * @throws java.lang.Exception failed to seek to the position
         */
        public synchronized int seek(int position) throws Exception {
            if (position < 0 || position >= size) throw new Exception("Invalid seek position.");
            return (this.position = position);
        }
        
        /**
         * Clears the block data
         */
        public synchronized void clear() {
            if (data != null) Arrays.fill(data, (byte) 0);
            reset();
        }
        
        /**
         * Resets the state of the block
         */
        public synchronized void reset() {
            position = 0;
        }
        
        /**
         * Checks for end of block
         * @return true if the end of block is reached
         */
        public synchronized boolean eob() {
            return (position >= size);
        }
        
        /**
         * Creates a clone of the block
         * @return the clone of the block
         */
        public Object clone() {
            Block block = new Block(size);
            if (data != null) {
                block.data = new byte[data.length];
                System.arraycopy(data, 0, block.data, 0, data.length);
            }
            block.size = size;
            block.position = position;
            return block;
        }
        
    }
    
}
