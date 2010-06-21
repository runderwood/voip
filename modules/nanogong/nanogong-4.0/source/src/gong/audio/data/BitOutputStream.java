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

import java.io.IOException;
import java.io.OutputStream;

/**
 * This class extends an output stream to read bit value.
 * @author Gibson Lam
 * @version 1.0, 12/01/2007
 */
public class BitOutputStream extends OutputStream {

    private OutputStream stream;
    private byte buffer;
    private int bptr;

    /**
     * Creates a new instance of BitOutputStream
     * @param stream the output stream
     */
    public BitOutputStream(OutputStream stream) {
        this.stream = stream;
        buffer = 0;
        bptr = 0;
    }

    /**
     * Writes a number of bits to the stream
     * @param b the number contains the bits
     * @param bits the number of bits to write
     * @throws java.io.IOException failed to write the bits
     */
    public void write(int b, int bits) throws IOException {
        for (int index = bits - 1; index >= 0; index--) {
            int mask = 1 << (7 - bptr);
            buffer |= mask;
            if ((b & (1 << index)) == 0) buffer ^= mask;
            bptr++;
            if (bptr == 8) {
                stream.write(buffer);
                buffer = 0;
                bptr = 0;
            }
        }
    }

    /**
     * Writes eight bits to the output stream
     * @param b the bits
     * @throws java.io.IOException failed to write the bits
     */
    public void write(int b) throws IOException {
        write(b, 8);
    }

    /**
     * Flushes the bitstream output
     * @throws java.io.IOException failed to flush the stream
     */
    public void flush() throws IOException {
        if (bptr > 0) {
            stream.write(buffer);
            buffer = 0;
            bptr = 0;
        }
    }

    /**
     * Closes the stream
     * @throws java.io.IOException failed to close the stream
     */
    public void close() throws IOException {
        flush();
    }

}
