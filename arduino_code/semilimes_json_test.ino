/***************************************************
The semilimes SDK connects microcontrollers to the semilimes server, enabling
secure instant messaging with forms and widgets. It supports interaction in
peer-to-peer chats, channels, and group chats, all while ensuring maximum
cybersecurity through the semilimes platform.

This project was designed to test the creation of all JSON scripts required by
the SDKs to enable the use of semilimes APIs. The scripts are printed via
serial output, and the example does not include a connection to the server.
The primary goal was to ensure that the generated scripts are correctly
formatted and ready for future integration with the semilimes platform.

This project is licensed under the MIT License.

Dependencies:
	semilimes by semilimes
 ****************************************************/

#include <Arduino.h>
//#include <./src/semilimes.h>
#include <src/semilimes.h>
      
void smeJsonTest()
{
    Serial.println("\r************** account ******************");    
    Serial.println("\rinit GetMyAccount");
    GetMyAccount getmyaccount;
    getmyaccount.set(true,false,true,false);
    Serial.println(getmyaccount.get());
    
    Serial.println("\rinit GetAccountProfile");
    GetAccountProfile getmyaccountprofile;
    getmyaccountprofile.set("abscnn11323");
    Serial.println(getmyaccountprofile.get());
    
    Serial.println("\rinit GetAccountFeed");
    GetAccountFeed getaccountfeed;
    getaccountfeed.set("abscnn11323",10,255,4987);
    Serial.println(getaccountfeed.get());
    
    Serial.println("\rinit AddAccountFeed");
    AddAccountFeed addaccountfeed;
    addaccountfeed.set("accFeed","add account feed", "avatar", true, false, true);
    addaccountfeed.addDataComponents("{\"dataComponentType\": \"text\",\"text\": \"string\"}");
    addaccountfeed.addDataComponents("{\"dataComponentType\": \"text\",\"text\": \"string\"}");
    addaccountfeed.addDataComponents("{\"dataComponentType\": \"text\",\"text\": \"string\"}");
    addaccountfeed.appendDataComponents();
    Serial.println(addaccountfeed.get());
    
    Serial.println("\rinit Provisioning");
    Provisioning provisioning;
    provisioning.set("devId","provKey");
    Serial.println(provisioning.get());
    Serial.println(provisioning.getRegisterDeviceURL());
    Serial.println(provisioning.getDeviceClaimStatusURL());
        
    Serial.println("\r************** form components ******************");
        
    Serial.println("\rinit FcBucketPicker");
    FcBucketPicker fcbucketpicker;
    fcbucketpicker.set("refname","title",true,"profile","actbuttontitle",true);
    fcbucketpicker.addValue("id1",fcbucketpicker.featureTypes[1]);
    fcbucketpicker.addValue("id2",fcbucketpicker.featureTypes[2]);
    fcbucketpicker.addValue("id3",fcbucketpicker.featureTypes[0]);
    fcbucketpicker.addValue("id4",fcbucketpicker.featureTypes[3]);
    fcbucketpicker.appendValue();
    Serial.println(fcbucketpicker.get());
    
    Serial.println("\rinit FcButtonList");
    FcButtonList fcbuttonlist;
    fcbuttonlist.set("myButtonList","Button List",false,"buttonName",true,fcbuttonlist.lineSize[0]);
    fcbuttonlist.addOptions("button1","choice1","icon1Name"); //add the first button (name, value, iconName)  
    fcbuttonlist.addOptions("button2","choice2","icon2Name"); //add the first button (name, value, iconName)  
    fcbuttonlist.addOptions("button3","choice3","icon3Name"); //add the first button (name, value, iconName)  
    fcbuttonlist.appendOptions(); //append the buttons to the "fcbuttonlist"
    Serial.println(fcbuttonlist.get());
    
    Serial.println("\rinit FcColorPicker");
    FcColorPicker fccolorpicker;
    fccolorpicker.set("refname","title",true,"value","Select Color");
    Serial.println(fccolorpicker.get());
    
    Serial.println("\rinit FcContactPicker");
    FcContactPicker fccontactpicker;
    fccontactpicker.set("refname","title",true,"value","actBtnTitle",true);
    Serial.println(fccontactpicker.get());
    
    Serial.println("\rinit FcLabel");
    FcLabel fclabel;
    fclabel.set("Label","Example Form on a newly created Channel");
    Serial.println(fclabel.get());    
    
    Serial.println("\rinit FcTextBox");
    FcTextBox fctextbox;
    fctextbox.set("refname","title","value",true);
    Serial.println(fctextbox.get());
    
    Serial.println("\rinit FcSwitch");
    FcSwitch fcswitch;
    fcswitch.set("refname","title",true);
    Serial.println(fcswitch.get());
    
    Serial.println("\rinit FcTimePicker");
    FcTimePicker fctimepicker;
    fctimepicker.set("refname","title",true,"value","actBtnTitle");
    Serial.println(fctimepicker.get());
    
    Serial.println("\rinit FcSlider");
    FcSlider fcslider;
    fcslider.set("refname","title",true,89,1,255,2);
    Serial.println(fcslider.get());
    
    Serial.println("\rinit FcDatePicker");
    FcDatePicker fcdatepicker;
    fcdatepicker.set("refname","title",true,"value","actBtnTitle");
    Serial.println(fcdatepicker.get());
    
    Serial.println("\rinit FcLocationPicker");
    FcLocationPicker fclocationpicker;
    fclocationpicker.set("refname","title",true,"value","actBtnTitle",true);
    Serial.println(fclocationpicker.get());
    
    Serial.println("\rinit FcHiddenValue");
    FcHiddenValue fchiddenvalue;
    fchiddenvalue.set("refname","value");
    Serial.println(fchiddenvalue.get());
    
    Serial.println("\rinit FcNfcReader");
    FcNfcReader fcnfcreader;
    fcnfcreader.set("refname","title",true,"value","actBtnTitle");
    Serial.println(fcnfcreader.get());
    
    Serial.println("\rinit FcQrScanner");
    FcQrScanner fcqrscanner;
    fcqrscanner.set("refname","title",true,"value","actBtnTitle",fcqrscanner.mode[2]);
    Serial.println(fcqrscanner.get());
    
    Serial.println("\rinit FcSingleChoice");
    FcSingleChoice fcsinglechoice;
    fcsinglechoice.set("FcSingleChoice","FcSingle Choice",false,fcsinglechoice.mode[1],"value");
    fcsinglechoice.addOptions("button1","choice1"); //add the first button (name, value)  
    fcsinglechoice.addOptions("button2","choice2"); //add the first button (name, value)  
    fcsinglechoice.addOptions("button3","choice3"); //add the first button (name, value)  
    fcsinglechoice.appendOptions(); //append the buttons to the "fcsinglechoice"
    Serial.println(fcsinglechoice.get());
    
    Serial.println("\rinit FcMultipleChoice");
    FcMultipleChoice fcmultiplechoice;
    fcmultiplechoice.set("FcMultipleChoice","FcMultipleChoice",false);
    fcmultiplechoice.addOptions("button1","choice1"); //add the first button (name, value)  
    fcmultiplechoice.addOptions("button2","choice2"); //add the first button (name, value)  
    fcmultiplechoice.addOptions("button3","choice3"); //add the first button (name, value)  
    fcmultiplechoice.appendOptions(); //append the buttons to the "fcmultiplechoice" 
    fcmultiplechoice.addValue("choice1");
    fcmultiplechoice.addValue("choice2");
    fcmultiplechoice.addValue("choice3");
    fcmultiplechoice.appendValues(); //append the buttons to the "fcmultiplechoice" 
    Serial.println(fcmultiplechoice.get());
   
    Serial.println("\rinit FcEventPicker");
    FcEventPicker fceventpicker;
    fceventpicker.set("refname","title",true,false,fceventpicker.eventsDisplayMode[1]);
    fceventpicker.addEvent("id",123456213,"title","description","referenceBucketId","{add Infos}");
    fceventpicker.addEvent("id2",223456213,"title2","description2","referenceBucketId2","{add Infos2}");
    fceventpicker.addEvent("id3",323456213,"title3","description3","referenceBucketId3","{add Infos3}");
    fceventpicker.appendEvents();
    fceventpicker.addValue("{\"id3\",323456213,\"title3\",\"description3\",\"referenceBucketId3\",\"add Infos3\"}");
    fceventpicker.addValue("{\"id7\",723456213,\"title7\",\"description7\",\"referenceBucketId7\",\"add Infos7\"}");
    fceventpicker.appendValues();
    Serial.println(fceventpicker.get());

    Serial.println("\rinit FcGauge");
    FcGauge fcgauge;
    fcgauge.set("refname","title",1,360,2,10,150,"km/h",fcgauge.pointer[1],"#ABABAB",fcgauge.progressBar[0],"#ABABAB");
    fcgauge.addSegment(0,50,"#CCDD11");
    fcgauge.addSegment(0,150,"#CDDD11");
    fcgauge.addSegment(0,250,"#CEDD11");
    fcgauge.addSegment(0,360,"#CFDD11");
    fcgauge.appendSegments();
    Serial.println(fcgauge.get());
    
    Serial.println("\r************** data components ******************");
    
    Serial.println("\rinit DcAppointment");
    DcAppointment dcappointment;
    dcappointment.set("title","description",12346,987645,false,12.3456,12.3456);
    Serial.println(dcappointment.get());
    
    Serial.println("\rinit DcChReference");
    DcChReference dcchreference;
    dcchreference.set("channelId");
    Serial.println(dcchreference.get());
        
    Serial.println("\rinit DcContact");
    DcContact dccontact;
    dccontact.addContactIds("123");
    dccontact.addContactIds("456");
    dccontact.addContactIds("789");
    dccontact.appendContactIds();
    Serial.println(dccontact.get());
    
    Serial.println("\rinit DcFile");
    DcFile dcfile;
    dcfile.addFileId("123");
    dcfile.addFileId("456");
    dcfile.addFileId("789");
    dcfile.appendFileIds();
    Serial.println(dcfile.get());
    
    Serial.println("\rinit DcForm");
    DcForm dcform;
    dcform.set("recId",dcform.featureType[0],true,true,"submit","form1",dcform.align[1],false, true);
    dcform.addFormComponents(fclocationpicker.get());
    dcform.addFormComponents(fcslider.get());
    dcform.addFormComponents(fcswitch.get());
    dcform.appendFormComponents(); //append Form Components
    Serial.println(dcform.get());
        
    Serial.println("\rinit DcGauge");  
    DcGauge dcgauge;
    dcgauge.set("title",1,360,2,10,150,"km/h",dcgauge.pointer[1],"#ABABAB",dcgauge.progressBar[0],"#ABABAB");
    dcgauge.addSegment(0,50,"#CCDD11");
    dcgauge.addSegment(0,150,"#CDDD11");
    dcgauge.addSegment(0,250,"#CEDD11");
    dcgauge.addSegment(0,360,"#CFDD11");
    dcgauge.appendSegments();
    Serial.println(dcgauge.get());
    
    Serial.println("\rinit DcHtmlText");
    DcHtmlText dchtmltext;
    dchtmltext.set("html text");
    Serial.println(dchtmltext.get());
    
    Serial.println("\rinit DcSimpleText");
    DcSimpleText dcsimpletext;
    dcsimpletext.set("text");
    Serial.println(dcsimpletext.get());
    
    Serial.println("\rinit DcLocation");
    DcLocation dclocation;
    dclocation.set("loc name",44.405650,8.946256);
    Serial.println(dclocation.get());
    
    Serial.println("\rinit DcTunnelReference");
    DcTunnelReference dctunnelreference;
    dctunnelreference.set("tunnelid");
    Serial.println(dctunnelreference.get());

    Serial.println("\rinit DcWebview");
    DcWebview dcwebview;
    dcwebview.set("dcwebview",true,dcwebview.viewSizeTypes[0],dcwebview.displayMode[1],"linkDisplayName");
    Serial.println(dcwebview.get());
    
    Serial.println("\rinit DcTileView");
    DcTileView dctileview;
    dctileview.set(3);
    dctileview.addColorTile("colortile","title",0,1,0,1,"color_rgb", dctileview.actionType[0],"dataComponents");
    dctileview.addIconTile("icontile","title",0,1,1,1,"iconName","iconColor","icon backgroundColor",dctileview.actionType[1],"webUrl");
    dctileview.addPhotoTile("phototile","title",0,1,2,1,"uuid",dctileview.actionType[2],"bucketId");
    dctileview.addPhotoUrlTile("photoUrltile","title",0,1,3,1,"url",dctileview.actionType[3],"resourceId","subPath");
    dctileview.addWebUrlTile("WebUrltile","title",1,1,0,1,"web url",dctileview.actionType[4],"responseTemplateId","receiverId",true);
    dctileview.addLiveWebUrlTile("LiveWebUrltile","title",1,1,1,1,"liveweb url",dctileview.actionType[3],"resourceId2","subPath2");
    dctileview.addResourceTile("resourcetile","title",1,1,2,1,"resourceId","subPath",dctileview.actionType[2],"bucketId2");
    dctileview.addSlideshowTile("slideShowtile","title",1,1,3,1,dctileview.actionType[1],"webUrl2");
    dctileview.addFormTile("formtile","title",2,1,0,1,"{\"form\":\"form.....\"}",dctileview.actionType[0],"dataComponents2");
    dctileview.appendTiles();
    Serial.println(dctileview.get());    
    
    Serial.println("\r************** communication ******************");
    Serial.println("\rinit BucketLike");
    BucketLike bucketlike;
    bucketlike.set("bucketID");
    Serial.println(bucketlike.getEPurl());
    Serial.println(bucketlike.get());

    Serial.println("\rinit BucketOpen");
    BucketOpen bucketopen;
    bucketopen.set("bucketID");
    Serial.println(bucketopen.getEPurl());
    Serial.println(bucketopen.get());
    
    Serial.println("\rinit BucketUnlike");
    BucketUnlike bucketunlike;
    bucketunlike.set("bucketID");
    Serial.println(bucketunlike.getEPurl());
    Serial.println(bucketunlike.get());
    
    Serial.println("\rinit BucketUpdate");
    BucketUpdate bucketupdate;
    bucketupdate.set("bucketID","title","description","avatar",true,false,true);
    bucketupdate.addDataComponents(dcchreference.get());
    bucketupdate.addDataComponents(dcchreference.get());
    bucketupdate.addDataComponents(dcchreference.get());
    bucketupdate.appendDataComponents();
    Serial.println(bucketupdate.getEPurl());
    Serial.println(bucketupdate.get());
    
    Serial.println("\rinit ChannelCreate");
    ChannelCreate channelcreate;
    channelcreate.set("title","avatar",true,false);
    channelcreate.addEditorsId("editorId1");
    channelcreate.addEditorsId("editorId2");
    channelcreate.addEditorsId("editorId3");
    channelcreate.addEditorsId("editorId4");
    channelcreate.appendEditorsIds();
    Serial.println(channelcreate.getEPurl());
    Serial.println(channelcreate.get());
   
    Serial.println("\rinit ChannelGet");
    ChannelGet channelget;
    channelget.set("ownerId","editorId","channelId","title");
    Serial.println(channelget.getEPurl());
    Serial.println(channelget.get());
    
    Serial.println("\rinit ChannelMessageGet");
    ChannelMessageGet channelmessageget;
    channelmessageget.set("channelId","messageId",10);
    Serial.println(channelmessageget.getEPurl());
    Serial.println(channelmessageget.get());
    
    Serial.println("\rinit ChannelMessageSend");
    ChannelMessageSend channelmessagesend;
    channelmessagesend.set("channelId",dcsimpletext.get(), true);
    Serial.println(channelmessagesend.getEPurl());
    Serial.println(channelmessagesend.get());

    Serial.println("\rinit ChannelMessageUpdate");
    ChannelMessageUpdate channelmessageupdate;
    channelmessageupdate.set("channelId",dcsimpletext.get(),true);
    Serial.println(channelmessageupdate.getEPurl());
    Serial.println(channelmessageupdate.get());
   
    Serial.println("\rinit ChannelSubscribe");
    ChannelSubscribe channelmessagesubscribe;
    channelmessagesubscribe.set("channelId");
    Serial.println(channelmessagesubscribe.getEPurl());
    Serial.println(channelmessagesubscribe.get());
   
    Serial.println("\rinit ChannelUnsubscribe");
    ChannelUnsubscribe channelmessageunsubscribe;
    channelmessageunsubscribe.set("channelId");
    Serial.println(channelmessageunsubscribe.getEPurl());
    Serial.println(channelmessageunsubscribe.get());
    
    Serial.println("\rinit ChannelsMyGet");
    ChannelsMyGet channelsmyget;
    channelsmyget.set(true,false,true);
    Serial.println(channelsmyget.getEPurl());
    Serial.println(channelsmyget.get());
    
    Serial.println("\rinit GroupchatCreate");
    GroupchatCreate groupchatcreate;
    groupchatcreate.set("title");
    groupchatcreate.addRecipientId("RecipientId1");
    groupchatcreate.addRecipientId("RecipientId2");
    groupchatcreate.addRecipientId("RecipientId3");
    groupchatcreate.addRecipientId("RecipientId4");
    groupchatcreate.appendRecipientIds();
    Serial.println(groupchatcreate.getEPurl());
    Serial.println(groupchatcreate.get());
    
    Serial.println("\rinit GroupchatInvite");
    GroupchatInvite groupchatinvite;
    groupchatinvite.set("groupChatId");
    groupchatinvite.addRecipientId("RecipientId1");
    groupchatinvite.addRecipientId("RecipientId2");
    groupchatinvite.addRecipientId("RecipientId3");
    groupchatinvite.addRecipientId("RecipientId4");
    groupchatinvite.appendRecipientIds();
    Serial.println(groupchatinvite.getEPurl());
    Serial.println(groupchatinvite.get());
    
    Serial.println("\rinit GroupchatMessageGet");
    GroupchatMessageGet groupchatmessageget;
    groupchatmessageget.set("groupChatId","messageId",10);
    Serial.println(groupchatmessageget.getEPurl());
    Serial.println(groupchatmessageget.get());
    
    Serial.println("\rinit GroupchatMessageReply");
    GroupchatMessageReply groupchatmessagereply;
    groupchatmessagereply.set("messageId",dcchreference.get());
    Serial.println(groupchatmessagereply.getEPurl());
    Serial.println(groupchatmessagereply.get());
    
    Serial.println("\rinit GroupchatMessageSend");
    GroupchatMessageSend groupchatmessagesend;
    groupchatmessagesend.set("groupChatId",dcsimpletext.get(),false);
    Serial.println(groupchatmessagesend.getEPurl());
    Serial.println(groupchatmessagesend.get());
    
    Serial.println("\rinit GroupchatMessageUpdate");
    GroupchatMessageUpdate groupchatmessageupdate;
    groupchatmessageupdate.set("messageId",dcsimpletext.get(),true);
    Serial.println(groupchatmessageupdate.getEPurl());
    Serial.println(groupchatmessageupdate.get());
    
    Serial.println("\rinit GroupchatReactionRemove");
    GroupchatReactionRemove groupchatmessageremove;
    groupchatmessageremove.set("messageId","reaction");
    Serial.println(groupchatmessageremove.getEPurl());
    Serial.println(groupchatmessageremove.get());
    
    Serial.println("\rinit GroupchatReactionSend");
    GroupchatReactionSend groupchatreactionsend;
    groupchatreactionsend.set("messageId","reaction");
    Serial.println(groupchatreactionsend.getEPurl());
    Serial.println(groupchatreactionsend.get());
   
    Serial.println("\rinit GroupchatsGet");
    GroupchatsGet groupchatget;
    groupchatget.addRecipientId("RecipientId1");
    groupchatget.addRecipientId("RecipientId2");
    groupchatget.addRecipientId("RecipientId3");
    groupchatget.addRecipientId("RecipientId4");
    groupchatget.appendRecipientIds();
    Serial.println(groupchatget.getEPurl());
    Serial.println(groupchatget.get());
    
    Serial.println("\rinit GroupchatSignalTyping");
    GroupchatSignalTyping groupchatsignaltyping;
    groupchatsignaltyping.set("groupChatId");
    Serial.println(groupchatsignaltyping.getEPurl());
    Serial.println(groupchatsignaltyping.get());
    
    Serial.println("\rinit P2pMessageGet");
    P2pMessageGet p2pmessageget;
    p2pmessageget.set("recipientId","messageId", 10);
    Serial.println(p2pmessageget.getEPurl());
    Serial.println(p2pmessageget.get());
    
    Serial.println("\rinit P2pMessageSend");
    P2pMessageSend p2pmessagesend;
    p2pmessagesend.set("recipientId",dcsimpletext.get(),true);
    Serial.println(p2pmessagesend.getEPurl());
    Serial.println(p2pmessagesend.get());
    
    Serial.println("\rinit P2pMessageReply");
    P2pMessageReply p2pmessagereply;
    p2pmessagereply.set("recipientId",dcchreference.get());
    Serial.println(p2pmessagereply.getEPurl());
    Serial.println(p2pmessagereply.get());
    
    Serial.println("\rinit P2pMessageUpdate");
    P2pMessageUpdate p2pmessageupdate;
    p2pmessageupdate.set("messageId",dcsimpletext.get(),true);
    Serial.println(p2pmessageupdate.getEPurl());
    Serial.println(p2pmessageupdate.get());
    
    Serial.println("\rinit P2pReactionRemove");
    P2pReactionRemove p2preactionremove;
    p2preactionremove.set("messageId","reaction");
    Serial.println(p2preactionremove.getEPurl());
    Serial.println(p2preactionremove.get());
    
    Serial.println("\rinit P2pReactionSend");
    P2pReactionSend p2preactionsend;
    p2preactionsend.set("messageId","reaction");
    Serial.println(p2preactionsend.getEPurl());
    Serial.println(p2preactionsend.get()); 
    
    Serial.println("\rinit P2pSignalTyping");
    P2pSignalTyping p2psignaltyping;
    p2psignaltyping.set("recipientId");
    Serial.println(p2psignaltyping.getEPurl());
    Serial.println(p2psignaltyping.get());    
}

void setup() 
{  
    Serial.begin(115200);
    while (!Serial);
    delay(200);

    smeJsonTest();
}

void loop() 
{
}
