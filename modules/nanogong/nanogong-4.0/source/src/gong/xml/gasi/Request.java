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
 
package gong.xml.gasi;

import org.w3c.dom.Document;

/**
 * This class contains an XML request for the script handler.
 * @version 1.0, 13/03/2006
 * @author Gibson Lam
 */
public class Request extends Message {
    
    /** The request names */
    protected static final String[] NAMES = {
        "PlayMediaRequest",
        "RecordMediaRequest",
        "PauseMediaRequest",
        "StopMediaRequest",
        "SetMediaTimeRequest",
        "GetMediaTimeRequest",
        "GetMediaDurationRequest",
        "SetMediaRateRequest",
        "GetMediaRateRequest",
        "GetMediaStatusRequest",
        "GetAudioLevelRequest",
        "MoveToPrevMessageRequest",
        "MoveToNextMessageRequest",
        "SelectMessageRequest",
        "GetCurrentMessageIdRequest",
        "SearchMessageRequest",
        "GetMessageRequest",
        "GetMessageContentRequest",
        "PostMessageRequest",
        "SaveMessageRequest",
        "PostToFormRequest",
        "LoadFromURLRequest",
        "GetCurrentTokenRequest",
        "GetBoardNameRequest",
        "GetBoardDataRequest",
        "GetVersionRequest"
    };
    
    /**
     * The request name
     */
    protected String name;
    
    /**
     * Creates a new instance of Request
     * @param name the request name
     * @throws java.lang.Exception failed to create the request
     */
    protected Request(String name) throws Exception {
        if (!isRequest(name)) throw new IllegalArgumentException();

        this.name = name;

        document.appendChild(document.createElementNS(NAMESPACE_URI, name));
        document.getDocumentElement().setAttribute("xmlns", NAMESPACE_URI);
    }
    
    /**
     * Creates a new instance of Request
     * @param document the XML document of the request
     */
    protected Request(Document document) {
        super(document);
        name = document.getDocumentElement().getLocalName();
    }
    
    /** Gets the name of the request
     * @return the request name
     */
    public String getName() {
        return name;
    }
    
    /**
     * Checks if the name is a request
     * @return true if the name is a request
     * @param name the name to be checked
     */
    public static boolean isRequest(String name) {
        for (int index = 0; index < NAMES.length; index++) {
            if (name.equals(NAMES[index])) return true;
        }
        return false;
    }
    
    /**
     * Checks if the name is a text request name
     * @return true if the name is a text request name
     * @param name the name to be checked
     */
    public static boolean isTextRequest(String name) {
        name = name + "Request";
        for (int index = 0; index < NAMES.length; index++) {
            if (name.equals(NAMES[index])) return true;
        }
        return false;
    }
    
}
