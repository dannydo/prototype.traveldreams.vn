
 package com.kissconcept.prototype;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import com.facebook.Session;

import android.app.NativeActivity;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.Signature;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import android.view.WindowManager;

public class MainActivity extends NativeActivity {

	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		try {
		    PackageInfo info = getPackageManager().getPackageInfo(
		            "ENTER.YOUR.PACKAGE.NAME", PackageManager.GET_SIGNATURES);
		    for (Signature signature : info.signatures) {
		        MessageDigest md = MessageDigest.getInstance("SHA");
		        md.update(signature.toByteArray());
		        Log.e("************** MY KEY HASH:",
		                Base64.encodeToString(md.digest(), Base64.DEFAULT));
		    }					
			
	    } catch (NameNotFoundException e) {

	    } catch (NoSuchAlgorithmException e) {

	    }
		
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		Session.getActiveSession().onActivityResult(this, requestCode, resultCode, data); 
	}
}

