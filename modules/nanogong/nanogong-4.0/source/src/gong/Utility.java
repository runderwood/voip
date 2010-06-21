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
 
package gong;

import gong.xml.GongDomObject;
import java.awt.Color;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.StringReader;
import java.io.StringWriter;
import java.security.MessageDigest;
import java.security.spec.KeySpec;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.DESKeySpec;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

/**
 * The utility class provides a set of useful functions for other classes.
 * @author Gibson Lam
 * @version 3.0, 13/08/2008
 */
public class Utility {
    
    private static DocumentBuilderFactory builderFactory = null;
    private static TransformerFactory transformerFactory = null;
    
    /**
     * Parses an XML string into an XML document
     * @param xmlString the XML string to be parsed
     * @return the XML document node of the XML string
     * @throws java.lang.Exception failed to parse the XML document
     */
    static public Document parseXMLDocument(String xmlString) throws Exception  {
        // Use the DOM Parser from Java
        if (builderFactory == null) builderFactory = GongDomObject.createDocumentBuilderFactory();
        DocumentBuilder builder = builderFactory.newDocumentBuilder();
        
        // Parse the XML string as a byte stream
        StringReader reader = new StringReader(xmlString);
        InputSource source = new InputSource(reader);
        
        // Get the document and remove the whitespace
        Document document = builder.parse(source);
        normalize(document);
        
        return document;
    }
    
    /**
     * Parses an XML file into an XML document
     * @param file the file to be parsed
     * @return the XML document node of the XML file
     * @throws java.lang.Exception failed to parse the XML document
     */
    static public Document parseXMLDocument(File file) throws Exception {
        // Use the DOM Parser from Java
        if (builderFactory == null) builderFactory = GongDomObject.createDocumentBuilderFactory();
        DocumentBuilder builder = builderFactory.newDocumentBuilder();
        
        // Parse the XML string as a input stream
        BufferedInputStream stream = new BufferedInputStream(new FileInputStream(file));
        InputSource source = new InputSource(stream);
        
        // Get the document and remove the whitespace
        Document document = builder.parse(source);
        normalize(document);
        
        return document;
    }
    
    /**
     * Creates a new DOM document
     * @return the document
     * @throws java.lang.Exception failed to create a new XML document
     */
    static public Document getNewDocument() throws Exception {
        if (builderFactory == null) builderFactory = GongDomObject.createDocumentBuilderFactory();
        DocumentBuilder builder = builderFactory.newDocumentBuilder();
        return builder.newDocument();
    }
    
    /**
     * Normalizes the DOM structure starting from the given node
     * @param node the node to start the normalization
     */
    protected static void normalize(Node node) {
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
     * Serializes an XML document into an XML string
     * @param document the document to be serialized
     * @param indented true if the XML is to be indented
     * @return the XML string
     * @throws java.lang.Exception failed to serialize the XML document
     */
    static public String serializeXMLDocument(Document document, boolean indented) throws Exception {
        // Get the writer
        StringWriter writer = new StringWriter();
        
        // prepare the source and result
        if (transformerFactory == null) transformerFactory = GongDomObject.createTransformerFactory();
        Transformer transformer = transformerFactory.newTransformer();
        DOMSource source = new DOMSource(document);
        StreamResult result = new StreamResult(writer);
        
        // Set the format of the output
        if (indented) {
            transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            transformer.setOutputProperty("{http://xml.apache.org/xalan}indent-amount", "2");
        } else
            transformer.setOutputProperty(OutputKeys.INDENT, "no");
        transformer.setOutputProperty(OutputKeys.ENCODING, "utf-8");
        
        // Transform the document
        transformer.transform(source, result);
        
        // Close the writer
        writer.close();
        
        // Return the xml string
        return writer.toString();
    }
    
    /** Serializes an XML document into an XML file
     * @param document the document to be serialized
     * @param indented true if the XML is to be indented
     * @param file the file to be output
     * @throws java.lang.Exception failed to serialize the XML document
     */
    static public void serializeXMLDocument(Document document, boolean indented, File file) throws Exception {
        // Retrieve the file stream
        FileOutputStream stream = new FileOutputStream(file);
        
        // prepare the source and result
        if (transformerFactory == null) transformerFactory = GongDomObject.createTransformerFactory();
        Transformer transformer = transformerFactory.newTransformer();
        DOMSource source = new DOMSource(document);
        StreamResult result = new StreamResult(stream);
        
        // Set the format of the output
        if (indented) {
            transformer.setOutputProperty(OutputKeys.INDENT, "yes");
            transformer.setOutputProperty("{http://xml.apache.org/xalan}indent-amount", "2");
        } else
            transformer.setOutputProperty(OutputKeys.INDENT, "no");
        transformer.setOutputProperty(OutputKeys.ENCODING, "utf-8");
        
        // Transform the document
        transformer.transform(source, result);
        
        // Close the stream
        stream.close();
    }
    
    /** Creates an XML element with tagName and text
     * @param document the XML document
     * @param tagName the tag name of the node
     * @param text the text inside the node
     * @param cdata true if a CDATA section is required
     * @return the XML element
     */
    static public Element createXMLElement(Document document, String tagName, String text, boolean cdata) {
        Element element = document.createElement(tagName);
        if (text != null && text.length() > 0) {
            if (cdata)
                element.appendChild(document.createCDATASection(text));
            else
                element.appendChild(document.createTextNode(text));
        }
        return element;
    }
    
    /** Gets an attribute from an XML element
     * @param element the XML element
     * @param attrName the attribute name
     * @return the value of the XML attribute
     */
    static public String getXMLAttribute(Element element, String attrName) {
        String attr = element.getAttribute(attrName);
        if (attr == null) return "";
        return attr.trim();
    }
    
    /** Gets the text from an XML element
     * @param element the XML element
     * @return the XML text
     */
    static public String getXMLText(Element element) {
        String text = "";
        NodeList nodeList = element.getChildNodes();
        for (int index = 0; index < nodeList.getLength(); index ++) {
            Node node = nodeList.item(index);
            if (node.getNodeType() == Node.TEXT_NODE)
                text += node.getNodeValue();
            else
                text += getXMLText((Element) node);
        }
        return text;
    }
    
    /** Replaces any escape characters with their HTML representations
     * @param input the string to be parsed
     * @return the escaped string
     */
    static public String escapeHTML(String input) {
        if (input == null) return "";
        
        String output = input;
        
        try {
            // The & character
            output = output.replaceAll("&", "&amp;");
            
            // The " character
            output = output.replaceAll("\"", "&quot;");
            
            // The ' character
            output = output.replaceAll("'", "&apos;");
            
            // The < character
            output = output.replaceAll("<", "&lt;");
            
            // The > character
            output = output.replaceAll(">", "&gt;");
            
            // roll back the &apos; string
            output = output.replaceAll("&apos;", "'");
        } catch (Exception e) {
            return output;
        }
        
        return output;
    }
    
    /** Formats a date to the format string
     * @param date the date
     * @param format the format string
     * @return the formatted date
     */
    static public String formatDate(Date date, String format) {
        // Format the date display
        return (new SimpleDateFormat(format, Locale.US)).format(date);
    }
    
    /** Format a time
     * @param time the time
     * @param showms true if millisec is required to be shown
     * @return the formatted date
     */
    static public String formatTime(long time, boolean showms) {
        double t = (double) time / 1000d;
        int sec = (int) t;
        int ms = (int) ((t - sec) * 1000);
        int min = sec / 60;
        sec = sec % 60;
        int hour = min / 60;
        min = min % 60;
        
        String text = "";
        DecimalFormat formatter = new DecimalFormat("00");
        if (hour > 0) text += formatter.format(hour) + ":";
        text += formatter.format(min) + ":" + formatter.format(sec);
        if (showms) {
            formatter = new DecimalFormat("000");
            text += "." + formatter.format(ms);
        }
        
        return text;
    }
    
    /** Encrypts a string using MD5
     * @param key the key
     * @param input the input string
     * @return the encrypted string
     */
    static public String encrypt(String key, String input) {
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            md.reset();
            md.update(key.getBytes("utf-8"));
            md.update(input.getBytes("utf-8"));
            return md.toString();
        } catch (Exception e) {
            return "";
        }
    }
    
    /** Encodes an URL based on RFC2396
     * @param url the url to be encoded
     * @return the encoded URL
     */
    static public String encodeURL(String url) {
        String encodedURL = "";
        String unsafeString = " \"<>#%{}|\\^~[]`";
        
        for (int i = 0; i < url.length(); i++) {
            char ch = url.charAt(i);
            if (ch >= 0 && ch <= 31 ||
                    ch >= 127 && ch <= 255 ||
                    unsafeString.indexOf(ch) != -1) {
                String hex = Integer.toHexString(ch);
                if (hex.length() == 1) hex = "0" + hex;
                encodedURL += "%" + hex;
            } else encodedURL += ch;
        }
        
        return encodedURL;
    }
    
    /**
     * Encrypts a string with a key
     * @return the encrypted string
     * @param text the text to be encrypted
     * @param key the key string
     * @param algorithm the name of the encryption algorithm
     * @throws java.lang.Exception failed to encrypt the string
     */
    public static String encryptString(String text, String key, String algorithm) throws Exception {
        // Generate the key
        while (key.length() < 8) key += "0";
        KeySpec keySpec = new DESKeySpec(key.getBytes());
        SecretKey secretKey = SecretKeyFactory.getInstance(algorithm).generateSecret(keySpec);
        
        // Init the cipher
        Cipher cipher = Cipher.getInstance(secretKey.getAlgorithm());
        cipher.init(Cipher.ENCRYPT_MODE, secretKey);
        
        byte[] result = cipher.doFinal(text.getBytes("utf-8"));
        
        return new sun.misc.BASE64Encoder().encode(result);
    }
    
    /**
     * Decrypts a string with a key
     * @return the decrypted string
     * @param text the text to be decrypted
     * @param key the key string
     * @param algorithm the name of the encryption algorithm
     * @throws java.lang.Exception failed to decrypt the string
     */
    public static String decryptString(String text, String key, String algorithm) throws Exception {
        // Generate the key
        while (key.length() < 8) key += "0";
        KeySpec keySpec = new DESKeySpec(key.getBytes());
        SecretKey secretKey = SecretKeyFactory.getInstance(algorithm).generateSecret(keySpec);
        
        // Init the cipher
        Cipher cipher = Cipher.getInstance(secretKey.getAlgorithm());
        cipher.init(Cipher.DECRYPT_MODE, secretKey);
        
        byte[] source = new sun.misc.BASE64Decoder().decodeBuffer(text);
        byte[] result = cipher.doFinal(source);
        
        return new String(result, "utf-8");
    }
    
    /** Converts a color into a hex string
     * @param color the color
     * @return the hex representation of the color in rrggbb
     */
    public static String colorToRGB(Color color) {
        if (color == null) return "000000";
        return Integer.toHexString(color.getRGB()).substring(2);
    }
    
    /** Converts a hex string into a color
     * @param rgb the rgb color
     * @return the color object
     */
    public static Color rgbToColor(String rgb) {
        try {
            return new Color(Integer.parseInt(rgb, 16) | 0xff000000);
        } catch (Throwable t) {
            return Color.BLACK;
        }
    }
    
}
