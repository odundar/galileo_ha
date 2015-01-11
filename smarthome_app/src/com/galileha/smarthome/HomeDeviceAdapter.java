/*
 * Copyright (C) Your copyright notice.
 *
 * Author: Onur Dundar
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package com.galileha.smarthome;

import java.util.ArrayList;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

/**
 * Custom Adapter for Home Automation ListView
 */
public class HomeDeviceAdapter extends BaseAdapter {

	private final ArrayList<DeviceItem> itemsArrayList;
	private LayoutInflater inflator;

	public HomeDeviceAdapter(Context context,
			ArrayList<DeviceItem> itemsArrayList) {
		super();
		this.itemsArrayList = itemsArrayList;
		inflator = LayoutInflater.from(context);
	}

	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return itemsArrayList.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return itemsArrayList.get(position);
	}

	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			convertView = inflator.inflate(R.layout.row_layout, null);
			holder = new ViewHolder();
			holder.deviceName = (TextView) convertView.findViewById(R.id.name);
			holder.deviceStatus = (TextView) convertView
					.findViewById(R.id.itemDescription);
			holder.deviceImage = (ImageView) convertView
					.findViewById(R.id.photo);

			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}

		holder.deviceName.setText(itemsArrayList.get(position).getDeviceName());
		holder.deviceStatus.setText(itemsArrayList.get(position)
				.getDeviceStatus());
		holder.deviceImage.setImageResource(itemsArrayList.get(position)
				.getDeviceImage());
		return convertView;
	}

	static class ViewHolder {
		TextView deviceName;
		TextView deviceStatus;
		ImageView deviceImage;
	}

}
