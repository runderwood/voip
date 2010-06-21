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
 
package gong.audio;

import javax.sound.sampled.AudioFormat;

/**
 * Class OlaBuffer filters an audio data with different rates using overlap-and-add.
 * @author Gibson Lam
 * @version 3.0 12/08/2008
 */
public class OlaBuffer {
    
    /**
     * The acceptance value for correlation calculation
     */
    protected final static float CORRELATION_ACCEPTANCE = 0.9f;
    
    /**
     * The audio format of the data
     */
    protected AudioFormat format;
    /**
     * The playback rate
     */
    protected float rate;
    /**
     * The size of the OLA buffer
     */
    protected int bufferSize;
    /**
     * The size of overlapping region between the OLA buffer and the next buffer
     */
    protected int overlapSize;
    /**
     * The current OLA buffer
     */
    protected int[] buffer;
    /**
     * The position in the buffer
     */
    protected int bufferPos;
    /**
     * The output buffer
     */
    protected int[] outputBuffer;
    /**
     * The position in the output buffer
     */
    protected int outputPos;
    /**
     * The windowing function for the overlapping region
     */
    protected float[] window;
    /**
     * The number of sample shift of the overlapping region in the OLA buffer
     */
    protected int sampleShift;
    /**
     * The start location of searching for shifting position
     */
    protected int searchStart;
    /**
     * The end location of searching for shifting position
     */
    protected int searchEnd;
    
    /**
     * Creates a new instance of OlaBuffer
     * @param format the audio format of the data
     * @param rate the playback rate
     */
    public OlaBuffer(AudioFormat format, float rate) {
        this.format = format;
        this.rate = rate;

        init();
    }

    /**
     * Sets the playback rate of the OLA buffer
     * @param rate the playback rate
     */
    public void setRate(float rate) {
        this.rate = rate;
        drain();
        init();
    }

    /**
     * Initializes the OLA buffer
     */
    public void init() {
        bufferSize = (int) (format.getSampleRate() / 20);
        buffer = new int[bufferSize];
        bufferPos = 0;

        if (rate == 1.0)
            sampleShift = 0;
        else {
            overlapSize = bufferSize / 2;
            window = new float[overlapSize];
            for (int index = 0; index < overlapSize; index++) window[index] = (float) index / (float) (overlapSize - 1);

            if (rate > 1.0) {
                sampleShift = (int) (-bufferSize * (rate - 1.0));
                if (sampleShift < -(bufferSize - overlapSize)) sampleShift = -(bufferSize - overlapSize);

                searchStart = sampleShift - overlapSize / 4;
                searchEnd = sampleShift + overlapSize / 4;
                if (searchStart < -overlapSize) searchStart = -overlapSize;
                if (searchEnd > 0) searchEnd = sampleShift;
            }
            else if (rate < 1.0) {
                sampleShift = (int) (bufferSize * (1.0 - rate));
                if (sampleShift > bufferSize - overlapSize) sampleShift = bufferSize - overlapSize;

                searchStart = sampleShift - overlapSize / 4;
                searchEnd = sampleShift + overlapSize / 4;
                if (searchStart < 0) searchStart = sampleShift;
                if (searchEnd > overlapSize) searchEnd = overlapSize;
            }
        }
    }
    
    /**
     * Finds the correlation value of a shifting position in the OLA buffer
     * @param left the left position
     * @param right the right position
     * @param size the size of the overlapping region
     * @return the correlation value
     */
    protected double findCorrelation(int left, int right, int size) {
        int x, y;
        double xy = 0, xx = 0, yy = 0;
        
        for (int index = 0; index < size; index++) {
            x = buffer[left + index];
            y = buffer[right + index];
            xy += x * y;
            xx += x * x;
            yy += y * y;
        }
        if (xy < 0) return 0;
        
        double det = xx * yy;
        if (det == 0) return 0;

        return (xy * xy) / det;
    }
    
    /**
     * Finds the shifting position when stretching the OLA buffer (slow down)
     * @return the number of shifts
     */
    protected int findStretchPosition() {
        int delta = 0;
        double corr, max;
        
        max = findCorrelation(0, sampleShift, overlapSize);
        if (max >= CORRELATION_ACCEPTANCE) return sampleShift;
        
        for (int shift = 1; shift < overlapSize / 4; shift++) {
            // left hand side
            if (sampleShift - shift >= searchStart) {
                corr = findCorrelation(0, sampleShift - shift, overlapSize);
                if (corr > max) {
                    max = corr;
                    delta = -shift;
                    if (max >= CORRELATION_ACCEPTANCE) break;
                }
            }
                    
            // right hand side
            if (sampleShift + shift <= searchEnd) {
                corr = findCorrelation(0, sampleShift + shift, overlapSize);
                if (corr > max) {
                    max = corr;
                    delta = shift;
                    if (max >= CORRELATION_ACCEPTANCE) break;
                }
            }
        }
        
        return sampleShift + delta;
    }
    
    /**
     * Finds the shifting position when shrinking the OLA buffer (speed up)
     * @return the number of shifts
     */
    protected int findShrinkPosition() {
        int delta = 0;
        int offset = bufferSize - overlapSize;
        double corr, max;
        
        max = findCorrelation(offset, offset + sampleShift, overlapSize);
        if (max >= CORRELATION_ACCEPTANCE) return sampleShift;
        
        for (int shift = 1; shift < overlapSize / 4; shift++) {
            // left hand side
            if (sampleShift - shift >= searchStart) {
                corr = findCorrelation(offset, offset + sampleShift - shift, overlapSize);
                if (corr > max) {
                    max = corr;
                    delta = -shift;
                    if (max >= CORRELATION_ACCEPTANCE) break;
                }
            }
                    
            // right hand side
            if (sampleShift + shift <= searchEnd) {
                corr = findCorrelation(offset, offset + sampleShift + shift, overlapSize);
                if (corr > max) {
                    max = corr;
                    delta = shift;
                    if (max >= CORRELATION_ACCEPTANCE) break;
                }
            }
        }
        
        return sampleShift + delta;
    }

    /**
     * Reads a sample from the OLA buffer
     * @return the current sample
     */
    public int read() {
        if (!isAvailable()) return 0;
        return outputBuffer[outputPos++];
    }

    /**
     * Writes a sample to the OLA buffer
     * @param sample the input sample
     */
    public void write(int sample) {
        buffer[bufferPos++] = sample;

        if (bufferPos == bufferSize) {
            int left, right;
            int shift;
            
            if (sampleShift == 0) {
                outputBuffer = new int[bufferSize];
                System.arraycopy(buffer, 0, outputBuffer, 0, bufferSize);
            }
            else if (sampleShift > 0) {
                shift = findStretchPosition();
                
                outputBuffer = new int[bufferSize + shift];
                
                // The unaffected buffer
                System.arraycopy(buffer, 0, outputBuffer, 0, shift);
                
                // The overlapped buffer
                for (int index = 0; index < overlapSize; index++) {
                    sample = (int) (buffer[index + shift] * (1f - window[index]) + buffer[index] * window[index]);
                    outputBuffer[shift + index] = sample;
                }
                
                // The shifted buffer
                System.arraycopy(buffer, overlapSize, outputBuffer, shift + overlapSize, bufferSize - overlapSize);
            }
            else if (sampleShift < 0) {
                shift = findShrinkPosition();

                outputBuffer = new int[bufferSize + shift];
                
                // The unaffected buffer
                System.arraycopy(buffer, 0, outputBuffer, 0, bufferSize - overlapSize + shift);

                // The overlapped buffer
                left = bufferSize - overlapSize + shift;
                right = bufferSize - overlapSize;
                for (int index = 0; index < overlapSize; index++, left++, right++) {
                    sample = (int) (buffer[left] * (1f - window[index]) + buffer[right] * window[index]);
                    outputBuffer[left] = sample;
                }
            }

            outputPos = 0;
            bufferPos = 0;
        }
    }

    /**
     * Checks if the output buffer is not empty
     * @return true if the output buffer is not empty
     */
    public boolean isAvailable() {
        return (outputBuffer != null && outputPos < outputBuffer.length);
    }

    /**
     * Drains the remaining buffer to the output buffer
     */
    public void drain() {
        if (bufferPos > 0) {
            outputBuffer = new int[bufferPos];
            System.arraycopy(buffer, 0, outputBuffer, 0, bufferPos);
            outputPos = 0;
            bufferPos = 0;
        }
    }
    
}
