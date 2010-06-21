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

import gong.Utility;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * This class contains a message (request/response/fault) for the script handler.
 * @version 1.0, 13/03/2006
 * @author Gibson Lam
 */
public class Message {
    
    /**
     * The namespace uri of the XML document
     */
    protected static final String NAMESPACE_URI = "http://gong.ust.hk/gasi10";
    
    /**
     * The XML document
     */
    protected Document document;
    
    /**
     * Creates a new instance of Message
     * @throws java.lang.Exception failed to create the message
     */
    protected Message() throws Exception {
        document = Utility.getNewDocument();
    }
    
    /**
     * Creates a new instance of Message
     * @param document the XML document of the message
     */
    protected Message(Document document) {
        this.document = document;
    }
    
    /**
     * Parses the message into one of the Request, Response or Fault
     * @param text the input xml text
     * @return a request, response or fault object
     * @throws java.lang.Exception failed to parse the string
     */
    public static Message parse(String text) throws Exception {
        Document document = Utility.parseXMLDocument(text);

        // Check the root element
        Element root = document.getDocumentElement(); 
        if (root == null) throw new Exception("Root element not found.");
        
        // Check the namespace
        if (root.getNamespaceURI() == null || !root.getNamespaceURI().equals(NAMESPACE_URI))
            throw new Exception("Namespace (" + NAMESPACE_URI + ") is not correct.");
        
        // Check the type and return
        String name = root.getLocalName();
        if (Request.isRequest(name))
            return new Request(document);
        else if (Response.isResponse(name))
            return new Response(document);
        else if (Fault.isFault(name))
            return new Fault(document);
        
        return null;
    }
    
    /**
     * Gets an instance of the request
     * @param name the request name
     * @return the request instance
     * @throws java.lang.Exception failed to create a request
     */
    public static Request newRequest(String name) throws Exception {
        return new Request(name);
    }
    
    /**
     * Gets an instance of the response
     * @param name the response name
     * @return the response instance
     * @throws java.lang.Exception failed to create a response
     */
    public static Response newResponse(String name) throws Exception {
        return new Response(name);
    }
    
    /**
     * Gets an instance of the fault
     * @param code the fault code
     * @param reason the fault reason
     * @return the fault instance
     * @throws java.lang.Exception failed to create a fault
     */
    public static Fault newFault(String code, String reason) throws Exception {
        return new Fault(code, reason);
    }
    
    /** Gets the document of the message
     * @return the document
     */
    public Document getDocument() {
        return document;
    }
    
    /**
     * Adds a parameter to the message, i.e. add an element to the root element
     * @param name the name of the value
     * @param value the text value
     * @param replace replace the old value
     * @throws java.lang.Exception failed to set the parameter
     */
    public void setParameter(String name, String value, boolean replace) throws Exception {
        Element root = document.getDocumentElement(); 
        if (root == null) throw new Exception("Root element not found.");

        // Find the old node with the same name
        Node oldNode = null;
        if (replace) {
           NodeList nodeList = root.getElementsByTagNameNS(NAMESPACE_URI, name);
           for (int index = 0; index < nodeList.getLength(); index++) {
               Node node = nodeList.item(index);
               if (node.getParentNode().equals(root)) oldNode = node;
           }
        }

        // Create the element
        Element element = document.createElementNS(NAMESPACE_URI, name);
        if (value != null && value.length() > 0) element.appendChild(document.createTextNode(value));

        if (oldNode == null)
           root.appendChild(element);
        else
           root.replaceChild(element, oldNode);
    }
    
    /**
     * Adds a parameter to the message, i.e. add an element to the root element
     * @param name the name of the value
     * @param value the root element of the parameter
     * @param replace replace the old value
     * @throws java.lang.Exception failed to set the parameter
     */
    public void setParameter(String name, Element value, boolean replace) throws Exception {
        Element root = document.getDocumentElement(); 
        if (root == null) throw new Exception("Root element not found.");

        // Find the old node with the same name
        Node oldNode = null;
        if (replace) {
           NodeList nodeList = root.getElementsByTagNameNS(NAMESPACE_URI, name);
           for (int index = 0; index < nodeList.getLength(); index++) {
               Node node = nodeList.item(index);
               if (node.getParentNode().equals(root)) oldNode = node;
           }
        }

        // Create the element
        Element element = document.createElementNS(NAMESPACE_URI, name);
        if (value != null) element.appendChild(value);

        if (oldNode == null)
           root.appendChild(element);
        else
           root.replaceChild(element, oldNode);
    }

    /**
     * Gets a parameter from the message
     * @param name the name of the value
     * @return the text value
     * @throws java.lang.Exception failed to set the parameter
     */
    public String getParameter(String name) throws Exception {
        Element root = document.getDocumentElement(); 
        if (root == null) throw new Exception("Root element not found.");

        // Find the node with the same name
        NodeList nodeList = root.getElementsByTagNameNS(NAMESPACE_URI, name);
        for (int index = 0; index < nodeList.getLength(); index++) {
           Node node = nodeList.item(index);
           if (node.getParentNode().equals(root)) {
               if (node.getFirstChild() == null ||
                   node.getFirstChild().getNodeType() != Node.TEXT_NODE)
                   return null;
               return node.getFirstChild().getNodeValue();
           }
        }

        return null;
    }

    /**
     * Gets a parameter from an element
     * @param parent the name of the element
     * @param name the name of the value
     * @return the text value
     * @throws java.lang.Exception failed to set the parameter
     */
    public String getParameter(String parent, String name) throws Exception {
        Element root = document.getDocumentElement(); 
        if (root == null) throw new Exception("Root element not found.");
        
        // Find the parent element
        NodeList parentList = root.getElementsByTagNameNS(NAMESPACE_URI, parent);
        for (int index = 0; index < parentList.getLength(); index++) {
            Element parentNode = (Element) parentList.item(index);
            
            // Find the node with the same name
            NodeList nodeList = parentNode.getElementsByTagNameNS(NAMESPACE_URI, name);
            for (int index2 = 0; index2 < nodeList.getLength(); index2++) {
               Node node = nodeList.item(index2);
               if (node.getParentNode().equals(parentNode)) {
                   if (node.getFirstChild() == null ||
                       node.getFirstChild().getNodeType() != Node.TEXT_NODE)
                       return null;
                   return node.getFirstChild().getNodeValue();
               }
            }
        }

        return null;
    }
    
    /** Returns the string representation of the message
     * @return the string
     */
    public String toString() {
        try {
            return Utility.serializeXMLDocument(document, true);
        }
        catch (Throwable t) {
            return null;
        }
    }
    
}
