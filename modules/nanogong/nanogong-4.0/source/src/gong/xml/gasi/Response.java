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
 * This class contains an XML response for the script handler.
 * @version 1.0, 13/03/2006
 * @author Gibson Lam
 */
public class Response extends Message {
    
    /** The request names */
    protected static final String[] NAMES = {
        "PlayMediaResponse",
        "RecordMediaResponse",
        "PauseMediaResponse",
        "StopMediaResponse",
        "SetMediaTimeResponse",
        "GetMediaTimeResponse",
        "GetMediaDurationResponse",
        "SetMediaRateResponse",
        "GetMediaRateResponse",
        "GetMediaStatusResponse",
        "GetAudioLevelResponse",
        "MoveToPrevMessageResponse",
        "MoveToNextMessageResponse",
        "SelectMessageResponse",
        "GetCurrentMessageIdResponse",
        "SearchMessageResponse",
        "GetMessageResponse",
        "GetMessageContentResponse",
        "PostMessageResponse",
        "SaveMessageResponse",
        "PostToFormResponse",
        "LoadFromURLResponse",
        "GetCurrentTokenResponse",
        "GetBoardNameResponse",
        "GetBoardDataResponse",
        "GetVersionResponse"
    };
    
    /**
     * The name of the response
     */
    protected String name;
    
    /**
     * Creates a new instance of Response
     * @param name the name of the response
     * @throws java.lang.Exception failed to create the response
     */
    protected Response(String name) throws Exception {
        if (!isResponse(name)) throw new IllegalArgumentException();

        this.name = name;

        document.appendChild(document.createElementNS(NAMESPACE_URI, name));
        document.getDocumentElement().setAttribute("xmlns", NAMESPACE_URI);
    }
    
    /**
     * Creates a new instance of Response
     * @param document the XML document of the response
     */
    protected Response(Document document) {
        super(document);
        name = document.getDocumentElement().getLocalName();
    }
    
    /** Gets the name of the response
     * @return the response name
     */
    public String getName() {
        return name;
    }
    
    /**
     * Checks if the name is a response
     * @param name the name to be checked
     * @return true if the name is a response
     */
    public static boolean isResponse(String name) {
        for (int index = 0; index < NAMES.length; index++) {
            if (name.equals(NAMES[index])) return true;
        }
        return false;
    }
    
}
