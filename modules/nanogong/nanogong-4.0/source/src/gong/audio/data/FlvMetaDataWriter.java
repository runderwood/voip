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

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Iterator;
import java.util.TreeMap;

/**
 * This class writes AMF data to an outputstream.
 * @version 1.0, 02/26/2007
 * @author Gibson Lam
 */
public class FlvMetaDataWriter {
    
    private TreeMap map = new TreeMap();
    
    /**
     * Creates a new instance of FlvMetaDataWriter
     */
    public FlvMetaDataWriter() {
    }
    
    /**
     * Sends the meta data to an output stream
     * @param stream the output stream
     * @throws java.io.IOException failed to send the meta data
     */
    public void sendToStream(OutputStream stream) throws IOException {
        BitOutputStream bitStream = new BitOutputStream(stream);
        
        bitStream.write(18);                                    // 0      : Signature (18)
        bitStream.write(size() - 11, 24);                       // 1  - 3 : Data size
        bitStream.write(0, 24);                                 // 4  - 6 : Timestamp (0)
        bitStream.write(0, 32);                                 // 7  - 10: Reserved (0)
        
        bitStream.write(2);                                     // 11     : String type (2)
        bitStream.write(10, 16);                                // 12 - 13: String length (10)
        bitStream.write(new String("onMetaData").getBytes());   // 14 - 23: "onMetaData"
        
        bitStream.write(8);                                     // 24     : Mixed array type (8)
        bitStream.write(map.size(), 32);                        // 25 - 28: Array size
        
        for (Iterator it = map.keySet().iterator(); it.hasNext();) {
            String key = (String) it.next();
            bitStream.write(key.length(), 16);                  // String length
            bitStream.write(key.getBytes());                    // String content
            
            Object object = map.get(key);
            if (object instanceof Integer ||
                    object instanceof Long ||
                    object instanceof Float ||
                    object instanceof Double) {
                double value = 0;
                
                if (object instanceof Integer) value = ((Integer) object).doubleValue();
                else if (object instanceof Long) value = ((Long) object).doubleValue();
                else if (object instanceof Float) value = ((Float) object).doubleValue();
                else value = ((Double) object).doubleValue();
                
                bitStream.write(0);                             // Number type (0)
                DataOutputStream dataStream = new DataOutputStream(bitStream);
                dataStream.writeDouble(value);                  // Value
            } else {
                String value = object.toString();
                
                bitStream.write(2);                             // String type (2)
                bitStream.write(value.length(), 16);            // String length
                bitStream.write(value.getBytes());              // String content
            }
        }
        
        bitStream.write(9, 24);                                 // End of metadata (9)
    }
    
    /**
     * Puts a key/value pair to the meta data
     * @param key the key
     * @param value the value
     */
    public void put(String key, Object value) {
        map.put(key, value);
    }
    
    /**
     * Returns the size of the meta data
     * @return the size
     */
    public int size() {
        int size = 29;
        
        for (Iterator it = map.keySet().iterator(); it.hasNext();) {
            String key = (String) it.next();
            
            size += 2 + key.length();
            
            Object object = map.get(key);
            if (object instanceof Integer ||
                    object instanceof Long ||
                    object instanceof Float ||
                    object instanceof Double) {
                size += 9;
            } else {
                String value = object.toString();
                
                size += 3 + value.length();
            }
        }
        
        size += 3;
        
        return size;
    }
    
}
