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
 * This class contains the fault object for the script handler.
 * @version 1.0, 13/03/2006
 * @author Gibson Lam
 */
public class Fault extends Message {
    
    /**
     * The constant of the fault name
     */
    protected static final String FAULT_NAME = "Fault";
    /**
     * The tag name of the code node
     */
    protected static final String CODE_NODE = "Code";
    /**
     * The tag name of the reason node
     */
    protected static final String REASON_NODE = "Reason";

    /**
     * The fault code
     */
    protected String code;
    /**
     * The fault reason
     */
    protected String reason;
    
    /**
     * Creates a new instance of Fault
     * @param code the fault code
     * @param reason the fault reason
     * @throws java.lang.Exception failed to create the fault
     */
    protected Fault(String code, String reason) throws Exception {
        this.code = code;
        this.reason = reason;

        document.appendChild(document.createElementNS(NAMESPACE_URI, FAULT_NAME));
        document.getDocumentElement().setAttribute("xmlns", NAMESPACE_URI);
        
        setParameter(CODE_NODE, code, true);
        setParameter(REASON_NODE, reason, true);
    }
    
    /**
     * Creates a new instance of Fault
     * @param document the XML document of the fault
     */
    protected Fault(Document document) {
        super(document);
        
        try {
            code = getParameter(CODE_NODE);
        }
        catch (Throwable t) {
            code = null;
        }
        try {
            reason = getParameter(REASON_NODE);
        }
        catch (Throwable t) {
            reason = null;
        }
    }
    
    /** Gets the code of the fault
     * @return the code
     */
    public String getCode() {
        return code;
    }
    
    /** Gets the reason of the fault
     * @return the reason
     */
    public String getReason() {
        return reason;
    }
    
    /**
     * Checks if the name is a fault
     * @return true if the name is a fault
     * @param name the fault name
     */
    public static boolean isFault(String name) {
        if (name.equals(FAULT_NAME)) return true;
        return false;
    }
    
}
