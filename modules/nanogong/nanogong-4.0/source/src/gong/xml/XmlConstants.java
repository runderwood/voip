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

/**
 * This is the constants for the dom object.
 * @author Gibson Lam
 * @version 1.0, 04/03/2006
 */
public class XmlConstants {
    
    /**
     * Namespace URI of the gong xml
     */
    public static final String NAMESPACE_URI = "http://gong.ust.hk/gong50";
    
    /**
     * The date format of the date fields
     */
    public static final String DATE_FORMAT = "yyyy-MM-dd";
    
    /**
     * The date/time format of the date fields
     */
    public static final String DATE_TIME_FORMAT = "yyyy-MM-dd HH:mm:ss";
    
    /**
     * The predefined text values
     */
    
    public static final String FULL = "full";
    public static final String HIDDEN = "hidden";
    public static final String NO = "no";
    public static final String NONE = "none";
    public static final String READ_ONLY = "read-only";
    public static final String VISIBLE = "visible";
    public static final String YES = "yes";
    
    private XmlConstants() {}
    
    /**
     * This class defines all tags for the gong xml files.
     */
    public static class Tag extends QName {
        
        /**
         * The predefined tags
         */
        
        public static final Tag ACC_CREATION = new Tag("accCreation");
        public static final Tag ACCESS_LIST = new Tag("accessList");
        public static final Tag AUTH = new Tag("auth");
        public static final Tag AUTHOR = new Tag("author");
        public static final Tag BASE_DN = new Tag("baseDN");
        public static final Tag BASIC = new Tag("basic");
        public static final Tag BOARD = new Tag("board");
        public static final Tag BOARD_LIST = new Tag("boardList");
        public static final Tag CAS = new Tag("cas");
        public static final Tag CATEGORY = new Tag("category");
        public static final Tag CHANNEL = new Tag("channel");
        public static final Tag CLIENT_IDLE_LIMIT = new Tag("clientIdleLimit");
        public static final Tag CONNECTION_STRING = new Tag("connectionString");
        public static final Tag CONTENT = new Tag("content");
        public static final Tag CONTEXT_MENU = new Tag("contextMenu");
        public static final Tag CONTEXT_MENU_ITEM = new Tag("contextMenuItem");
        public static final Tag COPYRIGHT = new Tag("copyright");
        public static final Tag DESCRIPTION = new Tag("description");
        public static final Tag DURATION = new Tag("duration");
        public static final Tag ENCLOSURE = new Tag("enclosure");
        public static final Tag ENCODED = new Tag("encoded");
        public static final Tag EXPLICIT = new Tag("explicit");
        public static final Tag GENERATOR = new Tag("generator");
        public static final Tag GLOBAL_CONNECTOR_LIMIT = new Tag("globalConnectorLimit");
        public static final Tag GLOBAL_WRITER_LIMIT = new Tag("globalWriterLimit");
        public static final Tag GROUP = new Tag("group");
        public static final Tag GROUP_LIST = new Tag("groupList");
        public static final Tag GUEST_PREFIX = new Tag("guestPrefix");
        public static final Tag GUID = new Tag("guid");
        public static final Tag HELP_URL = new Tag("helpUrl");
        public static final Tag HOMEPAGE = new Tag("homepage");
        public static final Tag HOST_URL = new Tag("hostUrl");
        public static final Tag ITEM = new Tag("item");
        public static final Tag LANGUAGE = new Tag("language");
        public static final Tag LDAP = new Tag("ldap");
        public static final Tag LINK = new Tag("link");
        public static final Tag LOGIN_URL = new Tag("loginUrl");
        public static final Tag MAXIMUM_CONNECTION = new Tag("maximumConnection");
        public static final Tag MAXIMUM_CONNECTION_ERROR = new Tag("maximumConnectionError");
        public static final Tag MAXIMUM_DURATION = new Tag("maximumDuration");
        public static final Tag MESSAGE = new Tag("message");
        public static final Tag MESSAGE_LIST = new Tag("messageList");
        public static final Tag NAME = new Tag("name");
        public static final Tag OWNER = new Tag("owner");
        public static final Tag PATH = new Tag("path");
        public static final Tag PERMISSION = new Tag("permission");
        public static final Tag PORT = new Tag("port");
        public static final Tag PUBDATE = new Tag("pubDate");
        public static final Tag RDF = new Tag("RDF");
        public static final Tag RSS = new Tag("rss");
        public static final Tag SERVER_SETTINGS = new Tag("serverSettings");
        public static final Tag SERVICE_URL = new Tag("serviceUrl");
        public static final Tag SESSION_CONNECTOR_LIMIT = new Tag("sessionConnectorLimit");
        public static final Tag SESSION_TIME_LIMIT = new Tag("sessionTimeLimit");
        public static final Tag SESSION_WRITER_LIMIT = new Tag("sessionWriterLimit");
        public static final Tag STREAMING = new Tag("streaming");
        public static final Tag SUBJECT = new Tag("subject");
        public static final Tag TITLE = new Tag("title");
        public static final Tag WEB_RESOURCE = new Tag("webResource");
        public static final Tag URL = new Tag("url");
        public static final Tag USER = new Tag("user");
        public static final Tag USER_ATTRIBUTE = new Tag("userAttribute");
        public static final Tag USER_LIST = new Tag("userList");
        
        /**
         * Creates a new instance of the Tag with a name
         * @param name the local name
         */
        public Tag(String name) {
            super(name);
        }
        
    }
    
    /**
     * This class defines all attributes for the gong xml files.
     */
    public static class Attribute extends QName {
        
        /**
         * The predefined attributes
         */
        
        public static final Attribute BOARD_ID = new Attribute("boardId");
        public static final Attribute BOARD_LIST_DISPLAY = new Attribute("boardListDisplay");
        public static final Attribute CODE = new Attribute("code");
        public static final Attribute COLOR = new Attribute("color");
        public static final Attribute CREATED_DATE = new Attribute("createdDate");
        public static final Attribute DURATION = new Attribute("duration");
        public static final Attribute FILESIZE = new Attribute("fileSize");
        public static final Attribute GROUP_USER_PERMISSION = new Attribute("groupUser");
        public static final Attribute GUEST_USER_PERMISSION = new Attribute("guestUser");
        public static final Attribute ID = new Attribute("id");
        public static final Attribute LENGTH = new Attribute("length");
        public static final Attribute MODIFIED_DATE = new Attribute("modifiedDate");
        public static final Attribute NAME = new Attribute("name");
        public static final Attribute NORMAL_USER_PERMISSION = new Attribute("normalUser");
        public static final Attribute OWNER = new Attribute("owner");
        public static final Attribute OWNER_EDITING = new Attribute("ownerEditing");
        public static final Attribute PARENT_ID = new Attribute("parentId");
        public static final Attribute PERMITTED_EDITING_LEVEL = new Attribute("permittedEditingLevel");
        public static final Attribute PUBLISH_RSS = new Attribute("publishRSS");
        public static final Attribute TYPE = new Attribute("type");
        public static final Attribute URL = new Attribute("url");
        public static final Attribute USER_LIST_DISPLAY = new Attribute("userListDisplay");
        public static final Attribute USERNAME = new Attribute("username");
        public static final Attribute VERSION = new Attribute("version");
        
        /**
         * Creates a new instance of the Attribute with a name
         * @param name the local name
         */
        public Attribute(String name) {
            super(name);
        }
        
    }
    
    /**
     * This class defines all qnames for the gong xml files.
     */
    private static class QName {
        
        private String localName;
        
        /**
         * Creates a new instance of the qname with a local name
         * @param name the local name
         */
        public QName(String name) {
            localName = name;
        }
        
        /**
         * Converts the qname to a qualified name
         * @param prefix the prefix of the qualified name
         * @return The qualified name
         */
        public String toQName(String prefix) {
            if (prefix == null)
                return localName;
            else
                return prefix + ":" + localName;
        }
        
        /**
         * Returns the string of qname
         * @return The local name
         */
        public String toString() {
            return localName;
        }
        
        /**
         * Compares the qname with another local name
         * @param name the local name to be compared
         * @return whether they match each other
         */
        public boolean equals(String name) {
            return localName.equals(name);
        }
        
    }
    
}
