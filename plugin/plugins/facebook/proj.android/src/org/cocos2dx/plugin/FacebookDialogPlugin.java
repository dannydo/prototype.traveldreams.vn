package org.cocos2dx.plugin;

import com.facebook.FacebookException;
import com.facebook.FacebookOperationCanceledException;
import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
import com.facebook.widget.FacebookDialog;
import com.facebook.widget.WebDialog;
import com.facebook.widget.WebDialog.OnCompleteListener;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.WindowManager;

public class FacebookDialogPlugin extends Dialog {

	private UiLifecycleHelper uiHelper;
	private static Activity mContext = null;
	private static UserFacebook mUserFacebook = null;
	
	public FacebookDialogPlugin(Context context, UserFacebook userFacebook) {
		super(context);
		// TODO Auto-generated constructor stub
		mContext = (Activity) context;
		mUserFacebook = userFacebook;
	}
	
	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        uiHelper = new UiLifecycleHelper(mContext, new Session.StatusCallback() {
            @Override
            public void call(Session session, SessionState state, Exception exception) {
                // Add code here to accommodate session changes
               
            }
        });
        
        uiHelper.onCreate(savedInstanceState);
    }
	
	public void shareFacebookDialog(String name, String caption, String description, String link, String picture) {
		if (FacebookDialog.canPresentShareDialog(mContext, FacebookDialog.ShareDialogFeature.SHARE_DIALOG)) {
			//Share app facebook dialog
			FacebookDialog shareDialog = new FacebookDialog.ShareDialogBuilder(mContext)
				.setName(name)
				.setCaption(caption)
				.setDescription(description)
		        .setLink(link)
		        .setPicture(picture)
		        .build();
			
			uiHelper.trackPendingDialogCall(shareDialog.present());
		}
		else {
			//Share web facebook dialog
		    Bundle params = new Bundle();
			params.putString(UserFacebook.Parameters.NAME, name);
		    params.putString(UserFacebook.Parameters.CAPTION, caption);
		    params.putString(UserFacebook.Parameters.DESCRIPTION, description);
		    params.putString(UserFacebook.Parameters.LINK, link);
		    params.putString(UserFacebook.Parameters.PICTURE, picture);
		    
			shareWebDialog(params);
		}
	}
	
	private void shareWebDialog(Bundle params) { 
	    WebDialog feedDialog = (new WebDialog.FeedDialogBuilder(mContext, Session.getActiveSession(), params))
    		.setOnCompleteListener(new OnCompleteListener() {
				
				@Override
				 public void onComplete(Bundle values, FacebookException error){
					// TODO Auto-generated method stub
					if (error == null) {
						// When the story is posted, echo the success and the post Id.
						final String postId = values.getString("post_id");
	                    if (postId != null) {
	                    	UserFacebook.LogD("Share success!");
	                    	UserWrapper.onActionResult(mUserFacebook, UserWrapper.ACTION_RET_LOGIN_SUCCEED, "Share success!");
	                    }
	                    else {
	                        // User clicked the Cancel button
	                    	UserFacebook.LogD("Share cancelled!");
	                    	UserWrapper.onActionResult(mUserFacebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share cancelled!");
	                    }
					}
					else if (error instanceof FacebookOperationCanceledException) {
						UserFacebook.LogD("Share cancelled!" + error.getMessage());
						UserWrapper.onActionResult(mUserFacebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share cancelled!");
					}
					else {
						UserFacebook.LogD("Share failed" + error.getMessage());
						UserWrapper.onActionResult(mUserFacebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share failed!");
					}
				}
			}).build();
	        
	    feedDialog.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING);
	    feedDialog.show();
	}
}
