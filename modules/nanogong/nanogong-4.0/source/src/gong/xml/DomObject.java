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

import gong.xml.XmlConstants.Attribute;
import gong.xml.XmlConstants.Tag;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.Vector;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.ProcessingInstruction;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 * This is the class for any object in the system to behave as a DOM
 * object. Several functions are included so that the object can be easily
 * parsed and serialized.
 * @version 1.0, 03/29/2006
 * @author Gibson Lam
 */
public abstract class DomObject implements Cloneable {
    
    private static DocumentBuilderFactory builderFactory = null;
    private static TransformerFactory transformerFactory = null;
    
    /** The anchor element of the Dom Object */
    protected Element anchor;
    
    /**
     * Creates an instance of the object
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     */
    public DomObject() throws ParserConfigurationException {
        // Create a new document
        Document document = createDocument();
        
        // Create the default anchor element
        anchor = document.createElement(getTag().toString());
        document.appendChild(anchor);
    }
    
    /**
     * Creates a new instance of the object
     *
     * @param object the element to be added as a anchor element
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     */
    public DomObject(Element object) throws InvalidTagException {
        anchor = object;
        validateElement();
    }
    
    /**
     * Creates a new instance of the object
     * @param string the xml string of the object
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     * @throws SAXException if parser error occurs.
     * @throws IOException if IO error occurs.
     */
    public DomObject(String string) throws ParserConfigurationException, InvalidTagException, SAXException, IOException {
        parse(string);
    }
    
    /**
     * Creates a new instance of the object
     * @param file the xml file of the object
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     * @throws SAXException if parser error occurs.
     * @throws IOException if IO error occurs.
     */
    public DomObject(File file) throws ParserConfigurationException, InvalidTagException, SAXException, IOException {
        parse(file);
    }
    
    /**
     * Gets the DOM document of the object
     * @return the document
     */
    public Document getDocument() {
        return anchor.getOwnerDocument();
    }
    
    /**
     * Gets the anchor element of the object
     * @return the anchor element
     */
    public Element getElement() {
        return anchor;
    }
    
    /**
     * Determines if the object is the document root
     * @return true if it is the document root
     */
    public boolean isDocumentRoot() {
        return (anchor.getParentNode().getNodeType() == Node.DOCUMENT_NODE);
    }
    
    /**
     * Gets the document root
     * @return the document root
     */
    public Element getDocumentRoot() {
        return getDocument().getDocumentElement();
    }
    
    /**
     * Validates the anchor element of the object
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     */
    protected void validateElement() throws InvalidTagException {
        // Retrieve the anchor tag
        Tag tag = getTag();
        if (tag == null) throw new InvalidTagException("Tag is not specified.");
        
        // Get the anchor element and validate it with namespace and tag name
        if (anchor == null) throw new InvalidTagException("Cannot find element: " + tag + ".");
        
        if (!tag.equals(anchor.getLocalName()))
            throw new InvalidTagException("Cannot find element: " + tag + ".");
    }
    
    /**
     * Gets the tag of the object
     * @return the tag of the object
     */
    protected abstract XmlConstants.Tag getTag();
    
    /**
     * Creates an object with an element
     * @param element the element
     * @return the object
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     */
    protected abstract DomObject createObject(Element element) throws InvalidTagException;
    
    /**
     * Parses an XML string into the object
     * @param string The XML string to be parsed
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     * @throws SAXException if parser error occurs.
     * @throws IOException if IO error occurs.
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     */
    public void parse(String string) throws ParserConfigurationException, SAXException, IOException, InvalidTagException {
        // Parse the XML string as a byte stream
        StringReader reader = new StringReader(string);
        parse(new InputSource(reader));
    }
    
    /**
     * Parses an XML file into the object
     * @param file The file to be parsed
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     * @throws SAXException if parser error occurs.
     * @throws IOException if IO error occurs.
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     * @throws FileNotFoundException if cannot locate the file.
     */
    public void parse(File file) throws ParserConfigurationException, SAXException, IOException, FileNotFoundException, InvalidTagException {
        // Parse the XML file as a input stream
        BufferedInputStream stream = new BufferedInputStream(new FileInputStream(file));
        parse(new InputSource(stream));
    }
    
    private static void setSystemProperty(String key, String[] values) {
        for (int index = 0; index < values.length; index++) {
            try {
                Class cls = Class.forName(values[index]);
                System.setProperty(key, values[index]);
                return;
            } catch (Throwable t) {}
        }
    }
    
    /**
     * Creates a document builder factory
     * @return the document builder factory
     */
    public static DocumentBuilderFactory createDocumentBuilderFactory() {
        setSystemProperty("javax.xml.parsers.DocumentBuilderFactory",
                new String[] {
            "org.apache.crimson.jaxp.DocumentBuilderFactoryImpl",
            "com.sun.org.apache.xerces.internal.jaxp.DocumentBuilderFactoryImpl"
        });
        DocumentBuilderFactory builderFactory = DocumentBuilderFactory.newInstance();
        builderFactory.setNamespaceAware(true);
        return builderFactory;
    }
    
    /**
     * Creates the transformer factory
     * @return the transformer factory
     */
    public static TransformerFactory createTransformerFactory() {
        setSystemProperty("javax.xml.transform.TransformerFactory",
                new String[] {
            "org.apache.xalan.processor.TransformerFactoryImpl",
            "com.sun.org.apache.xalan.internal.xsltc.trax.TransformerFactoryImpl"
        });
        setSystemProperty("org.apache.xml.dtm.DTMManager",
                new String[] {"org.apache.xml.dtm.ref.DTMManagerDefault"});
        setSystemProperty("com.sun.org.apache.xalan.internal.xsltc.dom.XSLTCDTMManager",
                new String[] {"com.sun.org.apache.xml.internal.dtm.ref.DTMManagerDefault"});
        TransformerFactory transformerFactory = TransformerFactory.newInstance();
        return transformerFactory;
    }
    
    /**
     * Parses an input source into the object
     * @param source The source to be parsed
     * @throws ParserConfigurationException if a suitable parser cannot be found.
     * @throws SAXException if parser error occurs.
     * @throws IOException if IO error occurs.
     * @throws InvalidTagException if the anchor element does not exist or not valid.
     */
    private void parse(InputSource source) throws ParserConfigurationException, SAXException, IOException, InvalidTagException {
        // Use the DOM Parser from Java
        if (builderFactory == null) builderFactory = createDocumentBuilderFactory();
        DocumentBuilder builder = builderFactory.newDocumentBuilder();
        
        // Parse the document and remove the whitespace
        Document document = builder.parse(source);
        normalize(document);
        
        // Validate the anchor element
        anchor = document.getDocumentElement();
        validateElement();
        
        // Remove the anchor default namespace declaration
        if (anchor.hasAttribute("xmlns")) anchor.removeAttribute("xmlns");
    }
    
    
    /**
     * Serializes the object into an XML string
     * @return The XML string
     * @throws TransformerConfigurationException if cannot get a transformer factory.
     * @throws TransformerException if cannot transform the document.
     * @throws IOException if cannot close the string.
     */
    public String serialize() throws TransformerConfigurationException, TransformerException, IOException {
        return serialize(anchor, true, "utf-8");
    }
    
    /**
     * Serializes an element into an XML string
     * @param node the node to be serialized
     * @return The XML string
     * @throws TransformerConfigurationException if cannot get a transformer factory.
     * @throws TransformerException if cannot transform the document.
     * @throws IOException if cannot close the string.
     */
    public String serialize(Node node) throws TransformerConfigurationException, TransformerException, IOException {
        return serialize(node, true, "utf-8");
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
        // Get the writer
        StringWriter writer = new StringWriter();

        // prepare the source and result
        if (transformerFactory == null) transformerFactory = createTransformerFactory();
        Transformer transformer = transformerFactory.newTransformer();
        DOMSource source = new DOMSource(node);
        StreamResult result = new StreamResult(writer);

        // Set the format of the output
        if (indented) {
            transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            transformer.setOutputProperty("{http://xml.apache.org/xalan}indent-amount", "2");
        } else
            transformer.setOutputProperty(OutputKeys.INDENT, "no");
        transformer.setOutputProperty(OutputKeys.ENCODING, encoding);

        // Transform the document
        transformer.transform(source, result);

        // Close the writer
        writer.close();

        // Return the xml string
        return writer.toString();
    }
    
    /**
     * Sets the id of the object
     * @param id the id of the object
     */
    public void setId(Long id) {
        String attr = (id == null)? null : String.valueOf(id);
        setTextAttribute(anchor, XmlConstants.Attribute.ID, attr);
    }
    
    /**
     * Gets the id of the object
     * @return the name of the object
     */
    public Long getId() {
        String id = getTextAttribute(anchor, XmlConstants.Attribute.ID);
        if (id == null) return null;
        try {
            return Long.decode(id);
        } catch (NumberFormatException ex) {
            return null;
        }
    }
    
    /**
     * Normalize the DOM structure starting from the given node
     * @param node the node to start the normalization
     */
    public static void normalize(Node node) {
        NodeList nodeList = node.getChildNodes();
        
        // Join any consecutive text nodes
        for (int index = 0; index < nodeList.getLength() - 1; index++) {
            Node child = nodeList.item(index);
            if (child.getNodeType() == Node.TEXT_NODE) {
                for (index++; index < nodeList.getLength(); index++) {
                    Node next = nodeList.item(index);
                    if (next.getNodeType() != Node.TEXT_NODE) break;
                    
                    String text = child.getNodeValue() + next.getNodeValue();
                    child.setNodeValue(text);
                    
                    node.removeChild(next);
                }
            }
        }
        
        nodeList = node.getChildNodes();
        
        // Remove any empty text nodes
        for (int index = 0; index < nodeList.getLength(); index++) {
            Node child = nodeList.item(index);
            if (child.getNodeType() == Node.TEXT_NODE) {
                String text = child.getNodeValue();
                if (text.trim().length() == 0) {
                    node.removeChild(child);
                    index--;
                }
            } else normalize(child);
        }
    }
    
    /**
     * Creates a new document
     * @return the newly created document
     * @throws javax.xml.parsers.ParserConfigurationException failed to create the document
     */
    protected Document createDocument() throws ParserConfigurationException {
        if (builderFactory == null) builderFactory = createDocumentBuilderFactory();
        DocumentBuilder builder = builderFactory.newDocumentBuilder();
        return builder.newDocument();
    }
    
    /**
     * Gets an element given an id (created because of the bug in Java crimson)
     * @param node the node starting the search
     * @param id the id
     * @return the matching element
     */
    public static Element getElementById(Node node, String id) {
        // Check itself
        if (node.getNodeType() == Node.ELEMENT_NODE) {
            Element element = (Element) node;
            String value = element.getAttribute(XmlConstants.Attribute.ID.toString());
            if (value.equals(id)) return element;
        }
        
        // Check children
        for (Node child = node.getFirstChild(); child != null; child = child.getNextSibling()) {
            if (child.getNodeType() == Node.ELEMENT_NODE) {
                Element element = getElementById(child, id);
                if (element != null) return element;
            }
        }
        
        return null;
    }
    
    /**
     * Gets a set of elements using the namespace and name
     * @param element the element to be searched
     * @param localName the targeted tagName
     * @param recursive whether to do a recursive search
     * @return A node list of tags matching the name
     */
    public static NodeList getElementsByTagName(Element element, String localName, boolean recursive) {
        // Use the DOM getElementsByTagNameNS for recursive search
        if (recursive) return element.getElementsByTagName(localName);
        
        // Prepare the returned nodelist implementation
        NodeListImpl match = new NodeListImpl();
        
        // Search the children for nodes matching the namespace and tag name
        for (Node node = element.getFirstChild(); node != null; node = node.getNextSibling()) {
            if (!localName.equals(node.getLocalName()))
                continue;
            
            match.add(node);
        }
        
        return match;
    }
    
    /**
     * Gets a set of elements using the namespace and name
     * @param element the element to be searched
     * @param namespaceURI the namespace URI
     * @param localName the targeted tagName
     * @param recursive whether to do a recursive search
     * @return A node list of tags matching the name
     */
    public static NodeList getElementsByTagNameNS(Element element, String namespaceURI, String localName, boolean recursive) {
        // Use the DOM getElementsByTagNameNS for recursive search
        if (recursive) return element.getElementsByTagNameNS(namespaceURI, localName);
        
        // Prepare the returned nodelist implementation
        NodeListImpl match = new NodeListImpl();
        
        // Search the children for nodes matching the namespace and tag name
        for (Node node = element.getFirstChild(); node != null; node = node.getNextSibling()) {
            if (!namespaceURI.equals(node.getNamespaceURI()))
                continue;
            if (!localName.equals(node.getLocalName()))
                continue;
            
            match.add(node);
        }
        
        return match;
    }
    
    /**
     * Gets the first node using the namespace
     * @return the node using the namespace
     * @param node the search node
     * @param namespaceURI the namespace URI
     */
    public static Node getNodeByNS(Node node, String namespaceURI) {
        // Check itself
        if (namespaceURI.equals(node.getNamespaceURI())) return node;
        
        // Check children
        for (Node child = node.getFirstChild(); child != null; child = child.getNextSibling()) {
            Node n = getNodeByNS(child, namespaceURI);
            if (n != null) return n;
        }
        
        return null;
    }
    
    /**
     * The inner class used to implement node list functionalities.
     */
    protected static class NodeListImpl extends Vector implements NodeList {
        
        /**
         * Returns a single item in the node list
         * @param index the item index
         * @return the node in the node list
         */
        public Node item(int index) {
            return (Node) get(index);
        }
        
        /**
         * Gets the size of the node list
         * @return the size of the list
         */
        public int getLength() {
            return size();
        }
        
    }
    
    /**
     * Sets the text element of the given target
     * @param element the target element
     * @param text the text value
     * @param type use cdata section (Node.CDATA_SECTION_NODE) or text node (Node.TEXT_NODE)
     */
    public static void setText(Element element, String text, short type) {
        if (type != Node.CDATA_SECTION_NODE && type != Node.TEXT_NODE)
            throw new IllegalArgumentException("Only cdata section or text node is allowed.");
        
        // Set or remove the node value
        NodeList nodeList = element.getChildNodes();
        for (int index = 0; index < nodeList.getLength(); index++) {
            Node node = nodeList.item(index);
            if (node.getNodeType() == type) {
                if (text != null) {
                    node.setNodeValue(text);
                    return;
                } else {
                    element.removeChild(node);
                    index--;
                }
            }
        }
        
        // Create the text element
        if (text != null) {
            if (type == Node.TEXT_NODE)
                element.appendChild(element.getOwnerDocument().createTextNode(text));
            else
                element.appendChild(element.getOwnerDocument().createCDATASection(text));
        }
    }
    
    /**
     * Gets the text element from the given target
     * @param element the target element
     * @param type use cdata section (Node.CDATA_SECTION_NODE) or text node (Node.TEXT_NODE)
     * @return the text data
     */
    public static String getText(Element element, short type) {
        if (type != Node.CDATA_SECTION_NODE && type != Node.TEXT_NODE)
            throw new IllegalArgumentException("Only cdata section or text node is allowed.");
        
        // Get the first text element
        for (Node node = element.getFirstChild(); node != null; node = node.getNextSibling()) {
            if (node.getNodeType() == type) return node.getNodeValue();
        }
        return null;
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
        NodeList nodeList = getElementsByTagName(parent, tag.toString(), false);
        if (nodeList.getLength() == 0) {
            element = (Element) parent.getOwnerDocument().createElement(tag.toString());
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
        NodeList nodeList = getElementsByTagName(parent, tag.toString(), false);
        if (nodeList.getLength() == 0) throw new TagNotFoundException("Cannot find the tag: " + tag + ".");
        
        return getText((Element) nodeList.item(0), type);
    }
    
    /**
     * Sets text to an attribute
     * @param element the element
     * @param attr the target attribute
     * @param text the text value
     */
    public static void setTextAttribute(Element element, Attribute attr, String text) {
        // Use attribute without NS because of a bug in Java 1.4.2
        if (text == null)
            element.removeAttribute(attr.toString());
        else
            element.setAttribute(attr.toString(), text);
    }
    
    /**
     * Gets text from an attribute
     * @param element the element
     * @param attr the target attribute
     * @return the text value
     */
    public static String getTextAttribute(Element element, Attribute attr) {
        // Use attribute without NS because of a bug in Java 1.4.2
        if (!element.hasAttribute(attr.toString())) return null;
        return element.getAttribute(attr.toString());
    }
    
    /**
     * Adds a style sheet for the document
     * @param href the style sheet location
     */
    public void addStyleSheet(String href) {
        Document document = getDocument();
        Node root = document.getDocumentElement();

        ProcessingInstruction pi = document.createProcessingInstruction("xml-stylesheet", "href=\"" + href + "\" type=\"text/xsl\"");
        root.getParentNode().insertBefore(pi, root);
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
