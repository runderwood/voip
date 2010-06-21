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
 
package gong.xml;

import gong.xml.XmlConstants.Tag;
import java.io.File;
import java.io.IOException;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/**
 * This is the class for any object in the system to behave as a DOM
 * object. Several functions are included so that the object can be easily
 * parsed and serialized.
 * @version 1.0, 03/29/2006
 * @author Gibson Lam
 */
public abstract class GongDomObject extends DomObject implements Cloneable {
    
    /**
     * Creates an instance of the object
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     */
    public GongDomObject() throws ParserConfigurationException {
        super();
        
        // Remove the previous anchor
        Document document = getDocument();
        document.removeChild(document.getDocumentElement());
        
        // Create the default anchor element
        anchor = document.createElementNS(XmlConstants.NAMESPACE_URI, getTag().toString());
        document.appendChild(anchor);
    }
    
    /**
     * Creates a new instance of the object
     * @param object the element to be added as a anchor element
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     */
    public GongDomObject(Element object) throws InvalidTagException {
        super(object);
    }
    
    /**
     * Creates a new instance of the object
     * @param string the xml string of the object
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     * @throws SAXException if parser error occurs.
     * @throws IOException if IO error occurs.
     */
    public GongDomObject(String string) throws ParserConfigurationException, InvalidTagException, SAXException, IOException {
        super(string);
    }
    
    /**
     * Creates a new instance of the object
     * @param file the xml file of the object
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     * @throws SAXException if parser error occurs.
     * @throws IOException if IO error occurs.
     */
    public GongDomObject(File file) throws ParserConfigurationException, InvalidTagException, SAXException, IOException {
        super(file);
    }
    
    /**
     * Validates the anchor element of the object
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     */
    protected void validateElement() throws InvalidTagException {
        super.validateElement();

        // Retrieve the anchor tag
        Tag tag = getTag();
        if (!XmlConstants.NAMESPACE_URI.equals(anchor.getNamespaceURI()))
            throw new InvalidTagException("Cannot find element: " + tag + ".");
    }
    
    /**
     * Serializes the object into an XML string
     * @param node the node to be serialized
     * @param indented True if the XML is to be indented
     * @param encoding The character encoding to be used
     * @return The XML string
     * @throws TransformerConfigurationException if cannot get a transformer factory.
     * @throws TransformerException if cannot transform the document.
     * @throws IOException if cannot close the string.
     */
    public String serialize(Node node, boolean indented, String encoding) throws TransformerConfigurationException, TransformerException, IOException {
        // Fix the namespace if not yet done
        Element ns = (Element) node;
        if (node != null) {
            String xmlns = "xmlns";
            if (ns.getPrefix() != null) xmlns += ":" + ns.getPrefix();
            ns.setAttributeNS("http://www.w3.org/2000/xmlns/", xmlns, ns.getNamespaceURI());
        }
        
        try {
            return super.serialize(node, indented, encoding);
        } finally {
            if (node != null) {
                String xmlns = "xmlns";
                if (ns.getPrefix() != null) xmlns += ":" + ns.getPrefix();
                ns.removeAttributeNS("http://www.w3.org/2000/xmlns/", xmlns);
            }
        }
    }
    
    /**
     * Sets text element to the target element
     * @param parent the target element
     * @param tag the tag of the text node
     * @param text the text value
     * @param type use cdata section (Node.CDATA_SECTION_NODE) or text node (Node.TEXT_NODE)
     */
    public static void setTextToElement(Element parent, Tag tag, String text, short type) {
        if (type != Node.CDATA_SECTION_NODE && type != Node.TEXT_NODE)
            throw new IllegalArgumentException("Only cdata section or text node is allowed.");
        
        Element element;
        NodeList nodeList = getElementsByTagNameNS(parent, XmlConstants.NAMESPACE_URI, tag.toString(), false);
        if (nodeList.getLength() == 0) {
            element = (Element) parent.getOwnerDocument().createElementNS(XmlConstants.NAMESPACE_URI, tag.toQName(parent.getPrefix()));
            parent.appendChild(element);
        } else element = (Element) nodeList.item(0);
        
        setText(element, text, type);
    }
    
    /**
     * Gets text element from the target element
     * @param parent the target element
     * @param tag the tag of the text node
     * @param type use cdata section (Node.CDATA_SECTION_NODE) or text node (Node.TEXT_NODE)
     * @return the text value
     * @throws TagNotFoundException if the tag does not exist.
     */
    public static String getTextFromElement(Element parent, Tag tag, short type) throws TagNotFoundException {
        NodeList nodeList = getElementsByTagNameNS(parent, XmlConstants.NAMESPACE_URI, tag.toString(), false);
        if (nodeList.getLength() == 0) throw new TagNotFoundException("Cannot find the tag: " + tag + ".");
        
        return getText((Element) nodeList.item(0), type);
    }
    
    /**
     * Clones the object
     * @return the cloned object
     * @throws CloneNotSupportedException if a list cannot be created.
     */
    public Object clone() throws CloneNotSupportedException {
        try {
            // Create a new document
            Document document = createDocument();

            // Import and add the anchor element
            document.appendChild(document.importNode(anchor, true));
            
            return createObject(document.getDocumentElement());
        } catch (ParserConfigurationException ex) {
            throw new CloneNotSupportedException("ParserConfigurationException: " + ex.getMessage());
        } catch (InvalidTagException ex) {
            throw new CloneNotSupportedException("InvalidTagException: " + ex.getMessage());
        }
    }
    
}
